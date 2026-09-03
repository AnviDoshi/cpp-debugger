#include "../include/codeeditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QDebug>
#include <QScrollBar>
#include <QAbstractItemView>

// ----------------------------------------------------------------------
// --- CodeEditor Implementation ---
// ----------------------------------------------------------------------

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->setFixedWidth(lineNumberAreaWidth());
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // Set font
    QFont font("Courier New", 10);
    setFont(font);
    setTabStopDistance(40);

    // ------------------------------------------------------------------
    // --- Auto Code Completion Setup -----------------------------------
    // ------------------------------------------------------------------
    keywordList << "int" << "float" << "double" << "char" << "string" << "bool"
                << "for" << "while" << "if" << "else" << "switch" << "case"
                << "return" << "break" << "continue" << "class" << "struct"
                << "public" << "private" << "protected" << "void" << "include"
                << "define" << "namespace" << "using" << "std" << "new"
                << "delete" << "try" << "catch" << "const" << "static"
                << "virtual" << "override" << "template" << "typename"
                << "this" << "nullptr" << "cout" << "cin" << "endl";

    completerModel = new QStringListModel(keywordList, this);
    completer = new QCompleter(completerModel, this);
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, &CodeEditor::insertCompletion);

    // ------------------------------------------------------------------
    // --- Auto Save + Recovery Feature ---------------------------------
    // ------------------------------------------------------------------
    autoSavePath = QDir::tempPath() + "/autosave.cpp";

    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(30000); // 30 seconds
    connect(autoSaveTimer, &QTimer::timeout, this, &CodeEditor::autoSaveCode);
    autoSaveTimer->start();

    // Restart autosave timer when user types
    connect(this, &QPlainTextEdit::textChanged, [this]() {
        if (autoSaveTimer->isActive())
            autoSaveTimer->start(); // restart 30s countdown
    });

    recoverLastSession();
}



// ----------------------------------------------------------------------
// --- Line Numbers and Layout ------------------------------------------
// ----------------------------------------------------------------------

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space + 40;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



// ----------------------------------------------------------------------
// --- Highlighting & Status Updates ------------------------------------
// ----------------------------------------------------------------------

void CodeEditor::highlightCurrentLine() 
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor("#3a3a3a").lighter(130);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    // Add error line highlights
    for (int lineNum : errorLines) {
        QTextEdit::ExtraSelection selection;
        QColor errorColor = QColor(Qt::red).lighter(160);
        selection.format.setBackground(errorColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        QTextCursor cursor(document()->findBlockByLineNumber(lineNum - 1));
        selection.cursor = cursor;
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
    emit textStatusChanged();
}



// ----------------------------------------------------------------------
// --- Line Number Gutter Rendering -------------------------------------
// ----------------------------------------------------------------------

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#252526"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    QFontMetrics fm(font());
    const int dotRadius = 10;
    const int dotMargin = 6;

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("#808080"));
            painter.drawText(0, top, lineNumberArea->width() - 18, fm.height(),
                             Qt::AlignRight, number);

            // Draw breakpoint dot if present
            int lineNum = blockNumber + 1;
            if (breakpoints.contains(lineNum)) {
                int centerX = lineNumberArea->width() - dotMargin - dotRadius;
                int centerY = top + fm.height() / 2;
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor("red"));
                painter.drawEllipse(QPoint(centerX, centerY), dotRadius, dotRadius);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}



// ----------------------------------------------------------------------
// --- Breakpoints & Execution Line -------------------------------------
// ----------------------------------------------------------------------

void CodeEditor::highlightErrorLine(int lineNumber)
{
    if (!errorLines.contains(lineNumber))
        errorLines.append(lineNumber);
    highlightCurrentLine();
}

void CodeEditor::clearErrorHighlights()
{
    errorLines.clear();
    highlightCurrentLine();
}

void CodeEditor::toggleBreakpointVisual(int line)
{
    if (line <= 0) return;
    if (breakpoints.contains(line))
        breakpoints.remove(line);
    else
        breakpoints.insert(line);
    lineNumberArea->update();
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    //
    // --- FIX for deprecated warning ---
    //
    if (event->button() == Qt::LeftButton && event->position().x() < lineNumberAreaWidth()) {
        QPoint clickPos = event->pos();
        QTextCursor cursor = cursorForPosition(clickPos);
        int lineNumber = cursor.blockNumber() + 1;
        emit breakpointToggled(lineNumber);
        toggleBreakpointVisual(lineNumber);
        return;
    }

    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::setExecutionLine(int lineNumber)
{
    QList<QTextEdit::ExtraSelection> extras;

    QTextCursor tc = textCursor();
    QTextEdit::ExtraSelection currentLineSel;
    currentLineSel.format.setProperty(QTextFormat::FullWidthSelection, true);
    currentLineSel.format.setBackground(QColor("#3a3a3a").lighter(130));
    currentLineSel.cursor = tc;
    currentLineSel.cursor.clearSelection();
    extras.append(currentLineSel);

    if (lineNumber >= 0) { // Only add execution line if it's valid
        QTextEdit::ExtraSelection execSel;
        execSel.format.setProperty(QTextFormat::FullWidthSelection, true);
        execSel.format.setBackground(QColor("#ffd54f"));
        QTextCursor execCursor(document()->findBlockByLineNumber(qMax(0, lineNumber - 1)));
        execSel.cursor = execCursor;
        execSel.cursor.clearSelection();
        extras.append(execSel);
    }

    for (int ln : errorLines) {
        QTextEdit::ExtraSelection errSel;
        errSel.format.setProperty(QTextFormat::FullWidthSelection, true);
        errSel.format.setBackground(QColor(255, 99, 71, 160));
        QTextCursor c(document()->findBlockByLineNumber(qMax(0, ln - 1)));
        errSel.cursor = c; errSel.cursor.clearSelection();
        extras.append(errSel);
    }

    setExtraSelections(extras);
}



// ----------------------------------------------------------------------
// --- Auto Code Completion Implementation ------------------------------
// ----------------------------------------------------------------------

void CodeEditor::insertCompletion(const QString &completion)
{
    if (completer->widget() != this)
        return;

    QTextCursor tc = textCursor();
    int extra = completion.length() - completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (completer && completer->popup()->isVisible()) {
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return;
        default:
            break;
        }
    }

    QPlainTextEdit::keyPressEvent(event);

    QString completionPrefix = textUnderCursor();

    if (completionPrefix.length() < 2) {
        completer->popup()->hide();
        return;
    }

    if (completionPrefix != completer->completionPrefix()) {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0)
                + completer->popup()->verticalScrollBar()->sizeHint().width());
    completer->complete(cr);
}



// ----------------------------------------------------------------------
// --- Auto Save + Session Recovery Implementation ----------------------
// ----------------------------------------------------------------------

void CodeEditor::autoSaveCode()
{
    QFile file(autoSavePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << toPlainText();
        file.close();
        qDebug() << "[AutoSave] Code saved to:" << autoSavePath;
    }
}

void CodeEditor::recoverLastSession()
{
    QFile file(autoSavePath);
    if (!file.exists())
        return;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString savedText = file.readAll();
        file.close();

        if (!savedText.isEmpty()) {
            setPlainText(savedText);
            qDebug() << "[AutoSave] Recovered last session from:" << autoSavePath;
        }
    }
}