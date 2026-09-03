#pragma once

#include <QMainWindow>
#include <QSet>
#include <QProcess>
#include <QTableWidget>
#include <QListWidget>

#include "codeeditor.h"
#include "geminiapi.h"
#include "complexityanalyzer.h"

#include <QStringList>

// Forward declarations
class QTextEdit;
class QLineEdit;
class QLabel;
class QTabWidget;
class QDockWidget;

//
// This struct holds the variable info
//
struct VariableInfo {
    QString name;
    QString value;
    QString memoryLocation; // We will use this to store the 'type'
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- File Operations ---
    void newFile();
    void openFile();
    void saveFile();

    // --- UI & Status ---
    void toggleTheme();
    void updateStatusBar();
    void applyTheme(int index);

    // --- AI & Analysis ---
    void onAnalyzeClicked();
    void onAnalysisComplete(const QList<ErrorInfo> &errors);
    void onAnalysisError(const QString &errorMessage);
    void onAnalyzeComplexityClicked();
    void onAIComplexityResponse(const QString &response);
    void onAiDebugClicked();

    //
    // --- FEATURE 3: AI-Powered Refactoring ---
    //
    void onRefactorClicked();

    // --- Code Execution & Debugging (REWIRED) ---
    void onRunClicked(); // "Start Debug"
    void onProcessOutput(); // Compiler output
    void onProcessFinished(int exitCode); // Compiler finished
    void startGdbSession(const QString& exePath); // Starts GDB
    void onGdbOutput(); // GDB stdout parser
    void onGdbInputEntered(); // GDB stdin
    void onGdbStepIn(); // Step In
    void onGdbStepOver(); // Step Over
    void onGdbContinue(); // Run to next breakpoint

    // --- Breakpoints ---
    void onBreakpointToggled(int line);

    void onChatInputReturnPressed();
void onChatResponseComplete(const QString &response);

private:
    void setupUI();
    void createToolBar();
    void createSidebar();

    void createChatbotDockWidget();

    // --- Core Components ---
    CodeEditor *codeEditor;
    GeminiAPI *geminiAPI;
    ComplexityAnalyzer complexityAnalyzer;
    QProcess *compilerProcess = nullptr;
    QProcess *gdbProcess = nullptr; // For GDB

    // --- File State ---
    QString currentFilePath;
    QString currentDebugExePath; // Path to the compiled .exe
    QSet<int> breakpoints;
    int currentExecutionLine = -1; // -1 means not paused
    
    //
    // --- FEATURE 3: Store AI suggestion ---
    //
    QString currentAiSuggestion;

    // --- UI Widgets ---
    QTabWidget *outputTabs;
    QTextEdit *errorDisplay;
    QTextEdit *outputConsole;
    QTextEdit *debugConsole; // GDB's raw MI output
    QTextEdit *aiExplainConsole;
    QLineEdit *inputLineEdit; // For STDIN to GDB
    QDockWidget *sidebarDock;
    QListWidget *fileExplorer;
    QTableWidget *variableTable;
    QListWidget *callStackWidget;



// +++ ADDED FOR CHATBOT +++
QDockWidget *chatDockWidget;
QTextEdit *chatHistoryView;
QLineEdit *chatInputLine;
QStringList chatHistory; // To store the conversation

    // --- Status Bar ---
    QLabel *statusLabel;
    QLabel *cursorPositionLabel;
    QLabel *wordCharCountLabel;
    QLabel *compilerStatusLabel;

    // --- Toolbar Actions ---
    QAction *analyzeButton;
    QAction *runButton; // "Debug"
    QAction *clearButton;
    QAction *stepInAction;
    QAction *stepOverAction;
    QAction *continueAction;
    QAction *aiDebugAction;

    //
    // --- FEATURE 3: Refactor Button ---
    //
    QAction *refactorAction;

    // --- Themes ---
    int currentThemeIndex;
    bool isDarkTheme;

    // --- Helpers ---
    void updateCompilerStatus(const QString &status);
    void updateVariablePanel(const QList<VariableInfo>& variables);
};