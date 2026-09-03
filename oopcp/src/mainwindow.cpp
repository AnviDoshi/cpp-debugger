#include "../include/mainwindow.h"
#include <QRegularExpression>
#include <QCoreApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <sstream> // For word count workaround
#include <QTextCursor>
#include <QDockWidget> // For Sidebar
#include <QListWidget> // For File Explorer
#include <QTabWidget>  // For Output Tabs
#include <QDebug>
#include <QDir> // For temp path

//
// --- Includes for 'incomplete type' errors ---
//
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTextBlock>
// ----------------------------------
#include <QTextDocument>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      currentThemeIndex(0),
      isDarkTheme(true)
{
    // Build the UI first (creates codeEditor, etc.)
    setupUI(); // This now creates inputLineEdit

    // Now create Gemini API and connect signals
    geminiAPI = new GeminiAPI(this);

    connect(geminiAPI, &GeminiAPI::analysisComplete,
        this, &MainWindow::onAnalysisComplete);

    connect(geminiAPI, &GeminiAPI::analysisError, this, &MainWindow::onAnalysisError);
    
    //
    // --- UI ENHANCEMENT: Render Markdown for AI Explain ---
    //
    connect(geminiAPI, &GeminiAPI::explanationComplete, this, [this](const QString &explanation) {
        // Convert the AI's Markdown response to HTML
        QTextDocument doc;
        doc.setMarkdown(explanation);
        
        // Wrap in a styled div for a "chat bubble" look
        QString html = QString("<div style='background-color: #2A2A2B; padding: 10px; border-radius: 5px; margin-top: 10px;'>"
                               "%1"
                               "</div>").arg(doc.toHtml());
        
        aiExplainConsole->append(html);
        statusLabel->setText("Status: AI explanation ready");
    });


    // +++ ADDED FOR CHATBOT +++
// Connect the new chat signals from the API
connect(geminiAPI, &GeminiAPI::chatResponseComplete, this, &MainWindow::onChatResponseComplete);
connect(geminiAPI, &GeminiAPI::chatResponseError, this, [this](const QString &errorMsg) {
    // Display the error in the chat window
    chatHistoryView->append(QString("<div style='color: #ff6b6b;'><b>Error:</b> %1</div>").arg(errorMsg));
    chatInputLine->setEnabled(true); // Re-enable input
});

    //
    // --- UI ENHANCEMENT: Render Markdown for AI Debug ---
    //
    connect(geminiAPI, &GeminiAPI::debugExplanationReady, this, [this](const QString &explanation) {
        // Convert the AI's Markdown response to HTML
        QTextDocument doc;
        doc.setMarkdown(explanation);

        // Wrap in a styled div
        QString html = QString("<div style='background-color: #2A2A2B; padding: 10px; border-radius: 5px; margin-top: 10px;'>"
                               "%1"
                               "</div>").arg(doc.toHtml());
                               
        aiExplainConsole->append(html);
        statusLabel->setText("Status: AI debug explanation ready.");
    });
    
    //
    // --- UI ENHANCEMENT: Format Refactor response ---
    //
    connect(geminiAPI, &GeminiAPI::refactorComplete, this, [this](const QString &refactoredCode) {
        
        // Create a styled block for the code
        QString codeBlockHtml = QString("<pre style=\"background-color: #1e1e1e; color: #d4d4d4; padding: 10px; border-radius: 5px; font-family: 'Courier New', monospace;\">%1</pre>")
                                    .arg(refactoredCode.toHtmlEscaped());

        // Wrap everything in a styled div
        QString html = QString("<div style='background-color: #2A2A2B; padding: 10px; border-radius: 5px; margin-top: 10px;'>"
                               "✅ <b>Refactor Complete!</b> Here is the suggested code:<br><br>"
                               "%1"
                               "</div>").arg(codeBlockHtml);
                               
        aiExplainConsole->append(html);
        statusLabel->setText("Status: Refactored code ready.");
        
        refactorAction->setEnabled(false); // Disable after use
        currentAiSuggestion.clear();
    });
    //
    // --- END UI ENHANCEMENTS ---
    //

    // --- GDB PROCESS ---
    gdbProcess = new QProcess(this);
    connect(gdbProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onGdbOutput);
    // Connection for STDIN is now in setupUI()

    // Now it's safe to connect codeEditor signals (codeEditor created by setupUI)
    connect(codeEditor, &CodeEditor::breakpointToggled, this, &MainWindow::onBreakpointToggled);
    connect(codeEditor, &CodeEditor::textStatusChanged, this, &MainWindow::updateStatusBar);

    setMinimumSize(900, 700);
    setWindowTitle("C++ Debugger - Untitled (Gemini AI)");
}

MainWindow::~MainWindow()
{
    // Clean up processes
    if (compilerProcess && compilerProcess->state() != QProcess::NotRunning) {
        compilerProcess->kill();
        compilerProcess->waitForFinished(500);
    }
    if (gdbProcess && gdbProcess->state() != QProcess::NotRunning) {
        gdbProcess->kill();
        gdbProcess->waitForFinished(500);
    }
}

// --- TOOLBAR ---

void MainWindow::createToolBar() {
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setIconSize(QSize(24, 24));
    toolbar->setStyleSheet("QToolBar { spacing: 10px; padding: 5px; background-color: #333333; border: none; }");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // --- File Actions ---
    QAction *newAction = toolbar->addAction(QIcon(":/icons/new.png"), "New");
    QAction *openAction = toolbar->addAction(QIcon(":/icons/open.png"), "Open");
    QAction *saveAction = toolbar->addAction(QIcon(":/icons/save.png"), "Save");

    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    toolbar->addSeparator();

    // --- Execution & Analysis Actions ---
    QAction *analyzeAction = toolbar->addAction(QIcon(":/icons/play.png"), "Analyze");
    
    QAction *runAction = toolbar->addAction(QIcon(":/icons/debug.png"), "Run & Debug");
    runAction->setToolTip("Compile and run with GDB");
    
    QAction *clearAction = toolbar->addAction(QIcon(":/icons/clear.png"), "Clear");
    
    // AI Explain Code Button
    QAction *explainAction = toolbar->addAction(QIcon(":/icons/ai.png"), "Explain");
    connect(explainAction, &QAction::triggered, this, [this]() {
        QString code = codeEditor->toPlainText().trimmed();
        if (code.isEmpty()) {
            QMessageBox::warning(this, "Empty Code", "Please enter some C++ code to explain!");
            return;
        }

        statusLabel->setText("Status: Requesting AI explanation...");
        aiExplainConsole->clear();
        
        //
        // --- UI ENHANCEMENT: "Asking" Message ---
        //
        aiExplainConsole->append("<div style='background-color: #3C3C3C; padding: 10px; border-radius: 5px;'>"
                               "🧠 <b>Asking AI to explain code...</b>"
                               "</div>");
                               
        outputTabs->setCurrentWidget(aiExplainConsole); // Switch to AI Explain tab
        geminiAPI->explainCode(code);
    });

    connect(analyzeAction, &QAction::triggered, this, &MainWindow::onAnalyzeClicked);
    connect(runAction, &QAction::triggered, this, &MainWindow::onRunClicked);
    connect(clearAction, &QAction::triggered, [this]() {
        errorDisplay->clear();
        outputConsole->clear();
        debugConsole->clear();
        aiExplainConsole->clear(); // Also clear AI tab
        codeEditor->clearErrorHighlights();
        statusLabel->setText("Status: Ready");
        // Clear debug UI
        variableTable->clearContents();
        variableTable->setRowCount(0);
        callStackWidget->clear();
        codeEditor->setExecutionLine(-1); // Clear execution highlight
        currentExecutionLine = -1; // Reset state
        aiDebugAction->setEnabled(false); // Disable AI debug
        refactorAction->setEnabled(false); // Disable refactor
        currentAiSuggestion.clear();
    });

    // Store the QAction pointers
    analyzeButton = analyzeAction;
    runButton = runAction;
    clearButton = clearAction;

    toolbar->addSeparator();

    // --- Debug Step Actions ---
    continueAction = toolbar->addAction(QIcon(":/icons/play.png"), "Continue"); 
    continueAction->setToolTip("Continue execution to next breakpoint");
    
    stepInAction = toolbar->addAction(QIcon(":/icons/step_in.png"), "Step In");
    stepOverAction = toolbar->addAction(QIcon(":/icons/step_over.png"), "Step Over");

    aiDebugAction = toolbar->addAction(QIcon(":/icons/ai.png"), "AI Debug Step");
    aiDebugAction->setToolTip("Ask AI to explain the current debug step");
    
    continueAction->setEnabled(false);
    stepInAction->setEnabled(false);
    stepOverAction->setEnabled(false);
    aiDebugAction->setEnabled(false); // Start disabled

    connect(continueAction, &QAction::triggered, this, &MainWindow::onGdbContinue);
    connect(stepInAction, &QAction::triggered, this, &MainWindow::onGdbStepIn);
    connect(stepOverAction, &QAction::triggered, this, &MainWindow::onGdbStepOver);
    connect(aiDebugAction, &QAction::triggered, this, &MainWindow::onAiDebugClicked);

    toolbar->addSeparator();

    QAction *complexityAction = toolbar->addAction(QIcon(":/icons/complexity.png"), "Complexity");
    connect(complexityAction, &QAction::triggered, this, &MainWindow::onAnalyzeComplexityClicked);
    
    refactorAction = toolbar->addAction(QIcon(":/icons/refactor.png"), "Refactor");
    refactorAction->setToolTip("Refactor the code based on the last complexity analysis");
    refactorAction->setEnabled(false); // Start disabled
    connect(refactorAction, &QAction::triggered, this, &MainWindow::onRefactorClicked);

    // --- Theme Toggle ---
    QAction *themeAction = toolbar->addAction(QIcon(":/icons/light.png"), "Theme");
    themeAction->setToolTip("Toggle Dark/Light Theme");
    themeAction->setCheckable(true);
    themeAction->setChecked(isDarkTheme);
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);

    // Apply button styling globally to toolbar actions
    toolbar->setStyleSheet(toolbar->styleSheet() +
                           "QToolButton {"
                           "    padding: 6px; border-radius: 6px;"
                           "    background-color: transparent;"
                           "}"
                           "QToolButton:hover {"
                           "    background-color: #555555;"
                           "}"
                           "QToolButton:checked {"
                           "    background-color: #007ACC;"
                           "}");
}

// --- SIDEBAR ---

void MainWindow::createSidebar() {
    sidebarDock = new QDockWidget("Project Explorer", this);
    sidebarDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QTabWidget *sidebarTabs = new QTabWidget(this);
    sidebarTabs->setObjectName("SidebarTabWidget"); // Give it an object name for selector

    fileExplorer = new QListWidget(this);
    fileExplorer->addItem("main.cpp (Active)");
    fileExplorer->addItem("codeeditor.cpp");
    fileExplorer->addItem("mainwindow.h");
    fileExplorer->setStyleSheet("QListWidget { background-color: #212121; color: #CCCCCC; border: none; }");

    // Index 0
    sidebarTabs->addTab(fileExplorer, QIcon(":/icons/folder.png"), "Explorer");

    // --- Debug Controls Tab ---
    variableTable = new QTableWidget(this);
    variableTable->setColumnCount(3);
    variableTable->setHorizontalHeaderLabels({"Variable", "Value", "Type"});
    variableTable->horizontalHeader()->setStretchLastSection(true); // Fills the space
    variableTable->verticalHeader()->setVisible(false);
    variableTable->setStyleSheet("QTableWidget { background-color: #1e1e1e; color: #d4d4d4; gridline-color: #3c3c3c; }"
                                 "QHeaderView::section { background-color: #333; color: #d4d4d4; }");

    // Index 1
    sidebarTabs->addTab(variableTable, QIcon(":/icons/watch.png"), "Variables");

    callStackWidget = new QListWidget(this);
    callStackWidget->setStyleSheet("QListWidget { background-color: #1e1e1e; color: #d4d4d4; border: none; }");
    
    // Index 2
    sidebarTabs->addTab(callStackWidget, QIcon(":/icons/stack.png"), "Call Stack");

    sidebarDock->setWidget(sidebarTabs);
    addDockWidget(Qt::LeftDockWidgetArea, sidebarDock);

    sidebarDock->setStyleSheet("QDockWidget { border-right: 1px solid #3c3c3c; }");
    sidebarDock->setMinimumWidth(200);
}

// --- COMPILATION & DEBUGGING ---

void MainWindow::onRunClicked()
{
    if (gdbProcess && gdbProcess->state() != QProcess::NotRunning) {
        gdbProcess->kill();
        gdbProcess->waitForFinished(200);
    }
    if (compilerProcess && compilerProcess->state() != QProcess::NotRunning) {
        compilerProcess->kill();
        compilerProcess->waitForFinished(200);
    }

    // Clear all debug UI
    outputConsole->clear();
    debugConsole->clear();
    variableTable->clearContents();
    variableTable->setRowCount(0);
    callStackWidget->clear();
    codeEditor->setExecutionLine(-1); 
    currentExecutionLine = -1; // Reset state
    stepInAction->setEnabled(false);
    stepOverAction->setEnabled(false);
    continueAction->setEnabled(false);
    aiDebugAction->setEnabled(false); // Disable AI debug
    refactorAction->setEnabled(false); // Disable refactor
    currentAiSuggestion.clear();

    if (!currentDebugExePath.isEmpty()) {
        QFile::remove(currentDebugExePath);
    }

    outputConsole->append("Compiling for debug...");
    
    QString sourcePath = QDir::temp().absoluteFilePath("temp_debug_run.cpp");
    currentDebugExePath = QDir::temp().absoluteFilePath(QString("temp_debug_%1.exe").arg(QCoreApplication::applicationPid()));
    
    QFile tempFile(sourcePath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        outputConsole->append("ERROR: Could not open temporary file for writing.");
        return;
    }
    QTextStream out(&tempFile);
    out << codeEditor->toPlainText();
    tempFile.close();

    if (!compilerProcess) {
        compilerProcess = new QProcess(this);
        connect(compilerProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessOutput);
        connect(compilerProcess, &QProcess::readyReadStandardError, this, &MainWindow::onProcessOutput);
        connect(compilerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), 
                this, &MainWindow::onProcessFinished);
    }

    QStringList args;
    args << sourcePath << "-g" << "-o" << currentDebugExePath;

    compilerProcess->start("g++", args);
    statusLabel->setText("Status: Compiling...");
}

void MainWindow::onProcessOutput()
{
    QByteArray stdOut = compilerProcess->readAllStandardOutput();
    if (!stdOut.isEmpty())
        outputConsole->append(QString::fromLocal8Bit(stdOut));

    QByteArray stdErr = compilerProcess->readAllStandardError();
    if (!stdErr.isEmpty())
        outputConsole->append(QString::fromLocal8Bit(stdErr));
}


void MainWindow::onProcessFinished(int exitCode)
{
    outputConsole->append("\nCompilation finished with exit code: " + QString::number(exitCode));
    if (exitCode == 0) {
        outputConsole->append("Compilation successful. Starting GDB...");
        if (!QFile::exists(currentDebugExePath)) {
            outputConsole->append("[ERROR] .exe does not exist!");
            statusLabel->setText("Status: Compile Error");
            return;
        }
        startGdbSession(currentDebugExePath);
    } else {
        outputConsole->append("Compilation failed. Fix errors and try again.");
        statusLabel->setText("Status: Compile Failed");
    }
}

void MainWindow::startGdbSession(const QString& exePath)
{
    if (gdbProcess->state() != QProcess::NotRunning) {
        gdbProcess->kill();
        gdbProcess->waitForFinished(200);
    }

    statusLabel->setText("Status: Starting GDB...");
    debugConsole->append("[SYSTEM] Starting GDB session...");
    outputTabs->setCurrentWidget(debugConsole); // Show GDB console

    QStringList args;
    args << "-q" << "--interpreter=mi"; // Use Machine Interface
    gdbProcess->start("gdb", args);

    if (!gdbProcess->waitForStarted()) {
        statusLabel->setText("Status: GDB Failed to start");
        debugConsole->append("[ERROR] GDB failed to start. Is it in your system's PATH?");
        QMessageBox::critical(this, "GDB Error", "GDB failed to start. Please ensure it is installed and in your system's PATH.");
        return;
    }

    gdbProcess->write(QString("-file-exec-and-symbols \"%1\"\n").arg(exePath).toUtf8());
    
    for (int line : breakpoints) {
        gdbProcess->write(QString("-break-insert \"%1:%2\"\n")
                          .arg(QDir::temp().absoluteFilePath("temp_debug_run.cpp"))
                          .arg(line)
                          .toUtf8());
    }

    gdbProcess->write("-exec-run\n");
    statusLabel->setText("Status: GDB is running...");
    
    // Enable step buttons
    stepInAction->setEnabled(true);
    stepOverAction->setEnabled(true);
    continueAction->setEnabled(true);
    aiDebugAction->setEnabled(false); // Can't use until stopped
}

/**
 * Handles STDIN - Called when user presses Enter in the inputLineEdit
 */
void MainWindow::onGdbInputEntered()
{
    QString input = inputLineEdit->text() + "\n";
    inputLineEdit->clear();
    inputLineEdit->setEnabled(false);

    outputConsole->moveCursor(QTextCursor::End);
    outputConsole->insertPlainText(input); // Echo the input
    outputConsole->moveCursor(QTextCursor::End);

    if (gdbProcess->state() == QProcess::Running) {
        gdbProcess->write(input.toUtf8());
    }
}

/**
 * Handles STDOUT - Called when gdbProcess sends new data
 */
void MainWindow::onGdbOutput()
{
    while (gdbProcess->canReadLine()) {
        QString line = gdbProcess->readLine().trimmed();
        if (line.isEmpty()) continue;
        
        debugConsole->append(line); // Log all GDB MI output

        if (line == "(gdb)") {
            // Ready for a new command
        }
        // Program's own output (stdout)
        else if (line.startsWith("~\"")) {
            QString output = line.mid(2);
            if (output.endsWith("\"")) output.chop(1);
            
            output.replace("\\n", "\n");
            output.replace("\\t", "\t");
            output.replace("\\\"", "\"");
            output.replace("\\\\", "\\");
            
            outputConsole->moveCursor(QTextCursor::End);
            outputConsole->insertPlainText(output);
            outputConsole->moveCursor(QTextCursor::End);

            if (!output.endsWith("\n")) {
                inputLineEdit->setEnabled(true);
                inputLineEdit->setFocus();
            } else {
                inputLineEdit->setEnabled(false);
            }
        }
        // GDB's log messages
        else if (line.startsWith("&\"")) {
            QString output = line.mid(2);
            if (output.endsWith("\"")) output.chop(1);
            
            output.replace("\\n", "\n");
            output.replace("\\t", "\t");
            output.replace("\\\"", "\"");
            output.replace("\\\\", "\\");
            
            outputConsole->moveCursor(QTextCursor::End);
            outputConsole->insertPlainText("[GDB] " + output);
            outputConsole->moveCursor(QTextCursor::End);
        }
        else if (line.startsWith("*running")) {
            statusLabel->setText("Status: Running...");
            stepInAction->setEnabled(true);
            stepOverAction->setEnabled(true);
            continueAction->setEnabled(true);
            aiDebugAction->setEnabled(false); // Can't use while running
            currentExecutionLine = -1; // Not paused
        }
        else if (line.startsWith("*stopped")) {
            statusLabel->setText("Status: Stopped");
            
            // --- ENHANCEMENT 1: Automatic Focus ---
            outputTabs->setCurrentWidget(outputConsole);
            QTabWidget* sidebarTabs = sidebarDock->widget()->findChild<QTabWidget*>("SidebarTabWidget");
            if (sidebarTabs) {
                sidebarTabs->setCurrentIndex(1); // 1 is "Variables"
            }
            // --- END ENHANCEMENT 1 ---

            QRegularExpression re("line=\"(\\d+)\"");
            QRegularExpressionMatch match = re.match(line); 
            if (match.hasMatch()) {
                int lineNum = match.captured(1).toInt();
                codeEditor->setExecutionLine(lineNum);
                
                currentExecutionLine = lineNum;
                aiDebugAction->setEnabled(true);
            }

            gdbProcess->write("-stack-list-variables 2\n"); // 2 = all values
            gdbProcess->write("-stack-list-frames\n");
            
            // Enable stepping
            stepInAction->setEnabled(true);
            stepOverAction->setEnabled(true);
            continueAction->setEnabled(true);
        }
        else if (line.startsWith("^exit") || line.startsWith("*stopped,reason=\"exited")) {
            statusLabel->setText("Status: Program finished.");
            stepInAction->setEnabled(false);
            stepOverAction->setEnabled(false);
            continueAction->setEnabled(false);
            aiDebugAction->setEnabled(false); // Can't use
            inputLineEdit->setEnabled(false);
            codeEditor->setExecutionLine(-1); // Clear execution line
            currentExecutionLine = -1; // Not paused
        }
        else if (line.startsWith("^done,variables=[")) {
            QList<VariableInfo> variables;
            // GDB Output: {name="a",type="int",value="10"}
            QRegularExpression re("\\{name=\"([^\"]+)\",type=\"([^\"]+)\",value=\"([^\"]*)\"\\}");
            QRegularExpressionMatchIterator it = re.globalMatch(line);
            while (it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                VariableInfo v;
                v.name = m.captured(1);
                v.value = m.captured(3); // Value is 3rd
                v.memoryLocation = m.captured(2); // Type is 2nd
                variables.append(v);
            }
            updateVariablePanel(variables);
        }
        else if (line.startsWith("^done,stack=[") || line.startsWith("^done,stack-frames=[") || line.startsWith("^done,stack=")) {
            QRegularExpression re("func=\"([^\"]+)\",file=\"([^\"]+)\",line=\"([^\"]+)\"");
            QRegularExpressionMatchIterator i = re.globalMatch(line);

            callStackWidget->clear();
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                if (match.hasMatch()) {
                    QString func = match.captured(1);
                    QString file = QFileInfo(match.captured(2)).fileName(); // Just get filename
                    QString lineNum = match.captured(3);
                    callStackWidget->addItem(QString("%1 — %2:%3").arg(func, file, lineNum));
                }
            }
        }
    }
}

// --- Debugging Step Functions ---

void MainWindow::onGdbContinue()
{
    if (gdbProcess && gdbProcess->state() == QProcess::Running) {
        gdbProcess->write("-exec-continue\n");
        statusLabel->setText("Status: Running...");
        stepInAction->setEnabled(false);
        stepOverAction->setEnabled(false);
        continueAction->setEnabled(false);
        aiDebugAction->setEnabled(false);
        currentExecutionLine = -1;
    }
}

void MainWindow::onGdbStepIn()
{
    if (gdbProcess && gdbProcess->state() == QProcess::Running) {
        gdbProcess->write("-exec-step\n");
        statusLabel->setText("Status: Stepping In...");
        stepInAction->setEnabled(false);
        stepOverAction->setEnabled(false);
        continueAction->setEnabled(false);
        aiDebugAction->setEnabled(false);
        currentExecutionLine = -1;
    }
}

void MainWindow::onGdbStepOver()
{
    if (gdbProcess && gdbProcess->state() == QProcess::Running) {
        gdbProcess->write("-exec-next\n");
        statusLabel->setText("Status: Stepping Over...");
        stepInAction->setEnabled(false);
        stepOverAction->setEnabled(false);
        continueAction->setEnabled(false);
        aiDebugAction->setEnabled(false);
        currentExecutionLine = -1;
    }
}


void MainWindow::onAiDebugClicked()
{
    // 1. Check if we are in a valid state
    if (currentExecutionLine == -1 || !gdbProcess || gdbProcess->state() != QProcess::Running) {
        QMessageBox::warning(this, "Not Paused", "You must be paused at a breakpoint to use AI Debug.");
        return;
    }

    // 2. Get current line of code
    QString lineText = codeEditor->document()->findBlockByLineNumber(currentExecutionLine - 1).text().trimmed();

    // 3. Get variables from the table
    QStringList varList;
    for (int i = 0; i < variableTable->rowCount(); ++i) {
        QString name = variableTable->item(i, 0)->text();
        if (name == "No variables in current scope") break;
        QString val = variableTable->item(i, 1)->text();
        QString type = variableTable->item(i, 2)->text();
        varList.append(QString("- %1 (%2) = %3").arg(name, type, val));
    }
    QString varString = varList.isEmpty() ? "No variables in scope." : varList.join("\n");

    // 4. Get call stack from the list
    QStringList stackList;
    for (int i = 0; i < callStackWidget->count(); ++i) {
        stackList.append(QString("- %1").arg(callStackWidget->item(i)->text()));
    }
    QString stackString = stackList.isEmpty() ? "Call stack is empty." : stackList.join("\n");

    // 5. Build the master prompt
    QString prompt = QString(
        "You are an expert C++ debugging assistant.\n"
        "The program is currently paused. Here is the complete context:\n\n"
        "**Current Line of Code (Line %1):**\n"
        "```cpp\n"
        "%2\n"
        "```\n\n"
        "**Local Variables in Scope:**\n"
        "%3\n\n"
        "**Call Stack (Top is most recent):**\n"
        "%4\n\n"
        "---"
        "**Your Task:**\n"
        "1.  **Explain WHAT** this line of code is about to do.\n"
        "2.  **Explain WHY** the program is here (e.g., \"it just entered the \'main\' function,\" or \"it\'s looping,\" or \"it\'s about to call a new function\").\n"
        "3.  **Predict** what will happen *after* this line executes (e.g., \"the variable \'c\' will be assigned the value 30,\" or \"the program will ask for user input\").\n"
        "Be clear, concise, and helpful. Use markdown.")
        .arg(currentExecutionLine)
        .arg(lineText)
        .arg(varString)
        .arg(stackString);

    // 6. Call the API
    statusLabel->setText("Status: Asking AI to explain debug step...");
    aiExplainConsole->clear();
    
    aiExplainConsole->append("<div style='background-color: #3C3C3C; padding: 10px; border-radius: 5px;'>"
                           "🧠 <b>Asking AI to explain the current step...</b>"
                           "</div>");
                           
    outputTabs->setCurrentWidget(aiExplainConsole);
    
    qDebug() << "AI Debug Prompt:\n" << prompt; // For our own debugging
    
    geminiAPI->explainDebugStep(prompt);
}



void MainWindow::onRefactorClicked()
{
    if (currentAiSuggestion.isEmpty() || codeEditor->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "No Suggestion", "Please run a Complexity Analysis first to get an AI suggestion.");
        return;
    }
    
    QString originalCode = codeEditor->toPlainText();
    
    // Build the refactor prompt
    QString prompt = "You are a C++ code refactoring expert.\n"
                     "Here is the original code:\n"
                     "```cpp\n" + originalCode + "\n```\n\n"
                     "Here is the analysis and suggestion you provided:\n"
                     "\"" + currentAiSuggestion + "\"\n\n"
                     "**Task:** Refactor the original code to implement the suggestion.\n"
                     "**IMPORTANT:** Respond with *only* the complete, refactored C++ code block. Do not add *any* other text, explanation, or markdown backticks around the code.";

    statusLabel->setText("Status: Asking AI to refactor code...");
    aiExplainConsole->clear();
    
    aiExplainConsole->append("<div style='background-color: #3C3C3C; padding: 10px; border-radius: 5px;'>"
                           "🤖 <b>Asking AI to refactor your code...</b><br>"
                           "This may take a moment."
                           "</div>");
                           
    outputTabs->setCurrentWidget(aiExplainConsole);
    
    geminiAPI->requestRefactor(prompt);
}



// --- THEME & UI ---

void MainWindow::toggleTheme() {
    currentThemeIndex = (currentThemeIndex + 1) % 3;
    QAction *themeAction = qobject_cast<QAction*>(sender());
    applyTheme(currentThemeIndex);

    if (themeAction) {
        if (currentThemeIndex == 0) {
            themeAction->setIcon(QIcon(":/icons/light.png"));
            themeAction->setText("Theme");
        } else if (currentThemeIndex == 1) {
            themeAction->setIcon(QIcon(":/icons/dark.png"));
            themeAction->setText("Theme");
        } else {
            themeAction->setIcon(QIcon(":/icons/solarized.png"));
            themeAction->setText("Theme");
        }
    }
}

void MainWindow::updateStatusBar() {
    QTextCursor cursor = codeEditor->textCursor();
    QString text = codeEditor->toPlainText();

    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    
    cursorPositionLabel->setText(QString("Line: %1, Col: %2").arg(line).arg(col));

    int charCount = text.length();
    
    //
    // --- FIX for std.string ---
    //
    std::string s = text.toStdString();
    
    std::stringstream ss(s);
    //
    // --- END FIX ---
    //

    std::string word;
    int wordCount = 0;
    while (ss >> word) {
        wordCount++;
    }

    wordCharCountLabel->setText(QString("Words: %1, Chars: %2").arg(wordCount).arg(charCount));
}

void MainWindow::setupUI()
{
    // 1. Setup Status Bar and Labels
    QStatusBar *mainStatusBar = new QStatusBar(this);
    setStatusBar(mainStatusBar);

    statusLabel = new QLabel("Status: Ready", this);
    cursorPositionLabel = new QLabel("Line: 1, Col: 1", this);
    wordCharCountLabel = new QLabel("Words: 0, Chars: 0", this);
    compilerStatusLabel = new QLabel("Compiler: OK", this);

    mainStatusBar->addWidget(statusLabel);
    mainStatusBar->addPermanentWidget(compilerStatusLabel);
    mainStatusBar->addPermanentWidget(wordCharCountLabel);
    mainStatusBar->addPermanentWidget(cursorPositionLabel);

    // 2. Create ToolBar
    createToolBar();

    // 3. CREATE SIDEBAR (Dock Widget)
    createSidebar();

    createChatbotDockWidget();

    // --- Central Widget Setup ---
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Splitter for code editor and output panel (Vertical Split)
    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, this);
    verticalSplitter->setHandleWidth(1);

    // Code Editor Section (Top Splitter Pane)
    QWidget *editorWidget = new QWidget(this);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *editorLabel = new QLabel("Enter Your C++ Code:", this);
    editorLabel->setStyleSheet("font-weight: bold; font-size: 12px; padding: 5px; background-color: #333333;");
    editorLayout->addWidget(editorLabel);

    codeEditor = new CodeEditor(this);
    codeEditor->setPlaceholderText("// Enter your C++ code here...\n#include <iostream>\nusing namespace std;\n\nint main() {\n    cout << \"Hello World!\" << endl;\n    return 0;\n}");
    editorLayout->addWidget(codeEditor);

    verticalSplitter->addWidget(editorWidget);

    // 4. Output Tab Panel (Bottom Splitter Pane)
    QWidget *outputWidget = new QWidget(this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputWidget);
    outputLayout->setContentsMargins(0, 0, 0, 0);

    outputTabs = new QTabWidget(this);

    // Initialize the member QTextEdits
    
    //
    // --- FIX for QTextText typo ---
    //
    errorDisplay = new QTextEdit(this);
    //
    // --- END FIX ---
    //

    outputConsole = new QTextEdit(this);
    debugConsole = new QTextEdit(this);

    errorDisplay->setReadOnly(true);
    outputConsole->setReadOnly(true); // This will show program output
    debugConsole->setReadOnly(true); // This will show GDB MI commands/output

    outputTabs->addTab(outputConsole, QIcon(":/icons/output.png"), "Output");
    outputTabs->addTab(errorDisplay, QIcon(":/icons/error.png"), "Errors");
    outputTabs->addTab(debugConsole, QIcon(":/icons/debug.png"), "Debug (GDB)");

   // AI Explanation tab
   aiExplainConsole = new QTextEdit(this);
   aiExplainConsole->setReadOnly(true);
   //
   // --- UI ENHANCEMENT: Set base styles for AI tab ---
   //
   aiExplainConsole->setStyleSheet("QTextEdit { "
                                   "  background-color: #1e1e1e; "
                                   "  color: #d4d4d4; "
                                   "  border: none; "
                                   "  padding: 5px; "
                                   "  font-family: 'Segoe UI', sans-serif; "
                                   "  font-size: 14px; "
                                   "  line-height: 1.6; "
                                   "}");
   outputTabs->addTab(aiExplainConsole, QIcon(":/icons/ai.png"), "AI Explain");

    outputLayout->addWidget(outputTabs);

    // --- STDIN ---
    inputLineEdit = new QLineEdit(this);
    inputLineEdit->setPlaceholderText("Enter input (stdin) for the program here and press Enter...");
    inputLineEdit->setStyleSheet("QLineEdit { "
                                 "  background-color: #1e1e1e; "
                                 "  color: #d4d4d4; "
                                 "  border: 1px solid #3c3c3c; "
                                 "  padding: 5px; "
                                 "}");
    inputLineEdit->setEnabled(false); // Start disabled
    outputLayout->addWidget(inputLineEdit);
    
    //
    // --- FIX for onGgdbInputEntered typo ---
    //
    connect(inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onGdbInputEntered);
    //
    // --- END FIX ---
    //
    
    // --- END STDIN ---

    verticalSplitter->addWidget(outputWidget);

    verticalSplitter->setStretchFactor(0, 7);
    verticalSplitter->setStretchFactor(1, 3);

    mainLayout->addWidget(verticalSplitter);

    // Connect Status Bar Update Signal
    connect(codeEditor, &CodeEditor::textStatusChanged, this, &MainWindow::updateStatusBar);

    // Apply Initial Theme
    applyTheme(currentThemeIndex);

    updateStatusBar();
}

// --- AI & ANALYSIS SLOTS ---

void MainWindow::onAnalyzeClicked()
{
    QString code = codeEditor->toPlainText().trimmed();

    if (code.isEmpty()) {
        QMessageBox::warning(this, "Empty Code", "Please enter some C++ code to analyze!");
        return;
    }

    statusLabel->setText("Status: Analyzing code with Gemini API...");
    QAction *analyzeAction = qobject_cast<QAction*>(analyzeButton);
    if (analyzeAction) analyzeAction->setEnabled(false);

    errorDisplay->clear();
    codeEditor->clearErrorHighlights();
    
    refactorAction->setEnabled(false); // Disable
    currentAiSuggestion.clear();

    // Call remote analyzer
    geminiAPI->analyzeCode(code);
}


//
// --- UI ENHANCEMENT: Rebuilt this function for the new "Error Card" UI ---
//
void MainWindow::onAnalysisComplete(const QList<ErrorInfo> &errors)
{
    analyzeButton->setEnabled(true);
    outputTabs->setCurrentWidget(errorDisplay); // Switch to errors tab

    if (errors.isEmpty()) {
        errorDisplay->setHtml("<div style='color: #4CAF50; font-weight: bold; font-size: 16px; padding: 10px;'>✓ No issues found! Your code looks good.</div>");
        statusLabel->setText("Status: Analysis complete - No issues found");
        return;
    }

    // HTML output is styled for a dark background for consistency
    QString htmlOutput = "<html><body style='font-family: Segoe UI, Arial, sans-serif; background-color: #1e1e1e; color: #d4d4d4; padding: 10px;'>";
    htmlOutput += "<h2 style='color: #ff6b6b; margin-bottom: 20px; border-bottom: 2px solid #ff6b6b; padding-bottom: 10px;'>🔍 Found " + QString::number(errors.size()) + " Issue(s):</h2>";

    for (int i = 0; i < errors.size(); ++i) {
        const ErrorInfo &error = errors[i];

        codeEditor->highlightErrorLine(error.lineNumber);

        QString errorColor;
        QString bgColor;
        QString icon;

        if (error.errorType.contains("Syntax", Qt::CaseInsensitive)) {
            errorColor = "#ff6b6b"; // Red
            bgColor = "#3d2626";
            icon = "❌";
        } else if (error.errorType.contains("Logic", Qt::CaseInsensitive)) {
            errorColor = "#ffa726"; // Orange
            bgColor = "#3d3226";
            icon = "⚠️";
        } else {
            errorColor = "#ab47bc"; // Purple
            bgColor = "#332640";
            icon = "🔧";
        }

        // --- This is the new, cleaner card layout ---
        htmlOutput += "<div style='background-color: " + bgColor + "; padding: 20px; margin: 15px 0; border-left: 6px solid " + errorColor + "; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.3);'>";
        
        // Header: Type + Line
        htmlOutput += "<h3 style='margin: 0 0 15px 0; color: " + errorColor + "; font-size: 18px;'>" + icon + " " + error.errorType + " on Line " + QString::number(error.lineNumber) + "</h3>";
        
        // Description
        htmlOutput += "<p style='margin: 10px 0; font-size: 14px; line-height: 1.6;'><strong style='color: #569cd6;'>Description:</strong><br/>"
                      "<span style='color: #ce9178;'>" + error.description.toHtmlEscaped() + "</span></p>";
                      
        // Suggested Fix (as code)
        htmlOutput += "<p style='margin: 10px 0; font-size: 14px; line-height: 1.6;'><strong style='color: #569cd6;'>Suggested Fix:</strong><br/>"
                      "<pre style='background-color: #1e1e1e; color: #4ec9b0; padding: 10px; display: block; border-radius: 5px; margin-top: 8px; font-family: Consolas, monospace;'>" + error.fix.toHtmlEscaped() + "</pre>"
                      "</p>";
                      
        htmlOutput += "</div>";
    }

    htmlOutput += "</body></html>";

    errorDisplay->setHtml(htmlOutput);
    statusLabel->setText("Status: Analysis complete - " + QString::number(errors.size()) + " issue(s) found");
}


void MainWindow::onAnalysisError(const QString &errorMessage)
{
    QAction *analyzeAction = qobject_cast<QAction*>(analyzeButton);
    if (analyzeAction) analyzeAction->setEnabled(true);

    errorDisplay->setHtml("<div style='color: red; font-weight: bold;'>Error: " + errorMessage + "</div>");
    statusLabel->setText("Status: Analysis failed");

    QMessageBox::critical(this, "Analysis Error", "Failed to analyze code:\n" + errorMessage);
}

// --- FILE OPERATIONS ---

void MainWindow::newFile() {
    codeEditor->clear();
    currentFilePath.clear();
    setWindowTitle("C++ Debugger - Untitled");
    statusLabel->setText("Status: New file created");
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open C++ File", "", "C++ Files (*.cpp *.h);;All Files (*.*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + fileName);
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    codeEditor->setPlainText(content);
    currentFilePath = fileName;
    setWindowTitle("C++ Debugger - " + QFileInfo(fileName).fileName());
    statusLabel->setText("Status: File opened successfully");
}

void MainWindow::saveFile() {
    QString fileName = currentFilePath;

    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save C++ File", "", "C++ Files (*.cpp *.h);;All Files (*.*)");
        if (fileName.isEmpty())
            return;
        currentFilePath = fileName;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not save file: " + fileName);
        return;
    }

    QTextStream out(&file);
    out << codeEditor->toPlainText();
    file.close();

    setWindowTitle("C++ Debugger - " + QFileInfo(fileName).fileName());
    statusLabel->setText("Status: File saved successfully");
}

// --- THEME & HELPERS ---

void MainWindow::applyTheme(int index) {
    QString mainStyle, editorStyle, statusBarStyle, errorDisplayStyle;
    QColor textColor;

    // --- 0. DARK THEME (Default) ---
    if (index == 0) {
        textColor = QColor("#d4d4d4");
        mainStyle = "QMainWindow { background-color: #252526; }";
        editorStyle = "QPlainTextEdit { background-color: #1e1e1e; color: #d4d4d4; border: 1px solid #3c3c3c; border-radius: 4px; padding: 5px; }";
        errorDisplayStyle = "QTextEdit { background-color: #1e1e1e; border: 1px solid #3c3c3c; color: #d4d4d4; padding: 5px; border-radius: 4px; }";
        statusBarStyle = "QStatusBar { background-color: #252526; color: #999; border-top: 1px solid #3d3d3d; }";
        sidebarDock->setStyleSheet("QDockWidget { border-right: 1px solid #3c3c3c; background-color: #212121; }");
    }
    // --- 1. LIGHT THEME ---
    else if (index == 1) {
        textColor = QColor("#000000");
        mainStyle = "QMainWindow { background-color: #f0f0f0; }";
        editorStyle = "QPlainTextEdit { background-color: white; color: black; border: 1px solid #ddd; border-radius: 4px; padding: 5px; }";
        errorDisplayStyle = "QTextEdit { background-color: white; border: 1px solid #ddd; color: black; padding: 5px; border-radius: 4px; }";
        statusBarStyle = "QStatusBar { background-color: #e0e0e0; color: #555; border-top: 1px solid #ddd; }";
        sidebarDock->setStyleSheet("QDockWidget { border-right: 1px solid #ddd; background-color: #f8f8f8; }");
    }
    // --- 2. SOLARIZED THEME (NEW) ---
    else { // index == 2
        textColor = QColor("#93a1a1");
        mainStyle = "QMainWindow { background-color: #002b36; }";
        editorStyle = "QPlainTextEdit { background-color: #002b36; color: #93a1a1; border: 1px solid #073642; border-radius: 4px; padding: 5px; }";
        errorDisplayStyle = "QTextEdit { background-color: #073642; border: 1px solid #073642; color: #93a1a1; padding: 5px; border-radius: 4px; }";
        statusBarStyle = "QStatusBar { background-color: #073642; color: #93a1a1; border-top: 1px solid #586e75; }";
        sidebarDock->setStyleSheet("QDockWidget { border-right: 1px solid #073642; background-color: #002b36; }");
    }

    setStyleSheet(mainStyle);
    codeEditor->setStyleSheet(editorStyle);
    errorDisplay->setStyleSheet(errorDisplayStyle);
    outputConsole->setStyleSheet(errorDisplayStyle); // Apply to other consoles
    debugConsole->setStyleSheet(errorDisplayStyle); // Apply to other consoles
    
    //
    // --- UI ENHANCEMENT: Update AI Tab style to match ---
    //
    if (index == 0) { // Dark
        aiExplainConsole->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; border: none; padding: 5px; font-family: 'Segoe UI', sans-serif; font-size: 14px; }");
    } else if (index == 1) { // Light
        aiExplainConsole->setStyleSheet("QTextEdit { background-color: #ffffff; color: #000000; border: none; padding: 5px; font-family: 'Segoe UI', sans-serif; font-size: 14px; }");
    } else { // Solarized
        aiExplainConsole->setStyleSheet("QTextEdit { background-color: #073642; color: #93a1a1; border: none; padding: 5px; font-family: 'Segoe UI', sans-serif; font-size: 14px; }");
    }
    //
    // --- END UI ENHANCEMENT ---
    //

    statusBar()->setStyleSheet(statusBarStyle);

    // Apply text color to status bar labels
    QString labelStyle = QString("color: %1; font-size: 11px;").arg(textColor.name());
    statusLabel->setStyleSheet(labelStyle);
    cursorPositionLabel->setStyleSheet(labelStyle);
    wordCharCountLabel->setStyleSheet(labelStyle);
    compilerStatusLabel->setStyleSheet(labelStyle);

    // Re-highlight the editor
    codeEditor->highlightCurrentLine();
}

void MainWindow::updateCompilerStatus(const QString &status) {
    compilerStatusLabel->setText(status);
}

void MainWindow::updateVariablePanel(const QList<VariableInfo>& variables)
{
    variableTable->clearContents(); // Use clearContents to keep headers
    variableTable->setColumnCount(3);
    
    variableTable->setHorizontalHeaderLabels({"Variable", "Value", "Type"});

    if (variables.isEmpty()) {
        variableTable->setRowCount(1);
        variableTable->setSpan(0, 0, 1, 3);
        QTableWidgetItem* msgItem = new QTableWidgetItem("No variables in current scope");
        msgItem->setFlags(msgItem->flags() & ~Qt::ItemIsEditable);
        msgItem->setTextAlignment(Qt::AlignCenter);
        variableTable->setItem(0, 0, msgItem);
    } else {
        variableTable->setRowCount(variables.size());
        for (int i = 0; i < variables.size(); ++i) {
            const auto& info = variables[i];
            variableTable->setItem(i, 0, new QTableWidgetItem(info.name));
            variableTable->setItem(i, 1, new QTableWidgetItem(info.value));
            
            // memoryLocation now holds the 'type'
            variableTable->setItem(i, 2, new QTableWidgetItem(info.memoryLocation)); 
        }
        variableTable->resizeColumnsToContents();
    }
}

void MainWindow::onBreakpointToggled(int line)
{
    bool gdbIsRunning = (gdbProcess && gdbProcess->state() == QProcess::Running);
    
    if (breakpoints.contains(line)) {
        breakpoints.remove(line);
        qDebug() << "Breakpoint removed at line:" << line;
        if (gdbIsRunning) {
            // This is complex, for now we just remove it from our set
            // GDB's breakpoint will remain until restart
        }
    } else {
        breakpoints.insert(line);
        qDebug() << "Breakpoint set at line:" << line;
        
        if (gdbIsRunning) {
            gdbProcess->write(QString("-break-insert \"%1:%2\"\n")
                              .arg(QDir::temp().absoluteFilePath("temp_debug_run.cpp"))
                              .arg(line)
                              .toUtf8());
        }
    }
}

void MainWindow::onAnalyzeComplexityClicked()
{
    QString code = codeEditor->toPlainText().trimmed();
    if (code.isEmpty()) {
        QMessageBox::warning(this, "Empty Code", "Please enter some C++ code first.");
        return;
    }

    // 1) Local heuristics
    ComplexityResult heur = complexityAnalyzer.analyze(code);

    // Display heuristic immediately in Output (so the user sees instant result)
    QString out = QString("<b>Heuristic estimate:</b><br>"
                          "Time: %1<br>Space: %2<br>Reason: %3<br><hr>")
                      .arg(heur.timeComplexity)
                      .arg(heur.spaceComplexity)
                      .arg(heur.reason);

    outputTabs->setCurrentWidget(outputConsole);
    outputConsole->clear();
    outputConsole->append(out);

    // 2) Ask the Gemini API for a detailed AI explanation
    connect(geminiAPI, &GeminiAPI::complexityAnalysisComplete, this, &MainWindow::onAIComplexityResponse);
    geminiAPI->analyzeComplexity(heur.promptForAI); 

    statusLabel->setText("Status: Asking AI for an in-depth complexity analysis...");
}

void MainWindow::onAIComplexityResponse(const QString &response)
{
    //
    // --- UI ENHANCEMENT: Render Markdown ---
    //
    QTextDocument doc;
    doc.setMarkdown(response);
    
    // Show AI response in Output panel
    outputConsole->append("<h3>AI Analysis</h3>");
    outputConsole->append(doc.toHtml()); // Append rendered HTML
    //
    // --- END UI ENHANCEMENT ---
    //
    
    statusLabel->setText("Status: AI complexity analysis ready.");
    
    //
    // --- FEATURE 3: Store suggestion and enable button ---
    //
    currentAiSuggestion = response; // Store the full suggestion
    refactorAction->setEnabled(true); // Enable the refactor button
    //
    // --- END FEATURE 3 ---
    //
    
    disconnect(geminiAPI, &GeminiAPI::complexityAnalysisComplete, this, &MainWindow::onAIComplexityResponse);
}









// +++ ADDED NEW FUNCTION for chat UI +++
void MainWindow::createChatbotDockWidget()
{
    chatDockWidget = new QDockWidget("AI Copilot", this);
    chatDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create the main widget and layout
    QWidget *chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setContentsMargins(5, 5, 5, 5);
    chatLayout->setSpacing(5);

    // 1. The chat history display
    chatHistoryView = new QTextEdit(this);
    chatHistoryView->setReadOnly(true);
    chatHistoryView->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; border: 1px solid #3c3c3c; }");
    
    // 2. The user input line
    chatInputLine = new QLineEdit(this);
    chatInputLine->setPlaceholderText("Ask the AI anything...");
    chatInputLine->setStyleSheet("QLineEdit { background-color: #252526; color: #d4d4d4; border: 1px solid #3c3c3c; padding: 5px; }");

    // 3. Add widgets to layout
    chatLayout->addWidget(chatHistoryView);
    chatLayout->addWidget(chatInputLine);

    // Set the layout on the main widget and set that as the dock's widget
    chatWidget->setLayout(chatLayout);
    chatDockWidget->setWidget(chatWidget);

    // Add the dock to the main window, on the right side
    addDockWidget(Qt::RightDockWidgetArea, chatDockWidget);

    // Connect the "Enter" key on the input line
    connect(chatInputLine, &QLineEdit::returnPressed, this, &MainWindow::onChatInputReturnPressed);
}


void MainWindow::onChatInputReturnPressed()
{
    QString message = chatInputLine->text().trimmed();
    if (message.isEmpty()) {
        return;
    }

    // Disable input while AI is thinking
    chatInputLine->clear();
    chatInputLine->setEnabled(false);

    // Display user's message (styled as a "user" bubble)
    QString userHtml = QString("<div style='background-color: #004a99; color: #ffffff; padding: 8px; border-radius: 5px; margin-left: 20px; margin-top: 5px;'>"
                             "<b>You:</b> %1"
                             "</div>").arg(message.toHtmlEscaped());
    chatHistoryView->append(userHtml);

    // Add to our simple history list
    chatHistory.append("USER: " + message);

    // Send to the API
    geminiAPI->sendChatMessage(chatHistory, message);
}

// This slot is called when the API signal fires
void MainWindow::onChatResponseComplete(const QString &response)
{
    // Add AI's response to our history
    chatHistory.append("MODEL: " + response);

    // Convert markdown response to HTML
    QTextDocument doc;
    doc.setMarkdown(response);

    // Display AI's message (styled as a "model" bubble)
    QString modelHtml = QString("<div style='background-color: #2A2A2B; padding: 8px; border-radius: 5px; margin-right: 20px; margin-top: 5px;'>"
                              "%1"
                              "</div>").arg(doc.toHtml());
    chatHistoryView->append(modelHtml);

    // Re-enable the input line
    chatInputLine->setEnabled(true);
    chatInputLine->setFocus();
}