#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QSet>
#include <QCompleter>
#include <QStringListModel>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QTextStream>

class CppHighlighter;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void highlightErrorLine(int lineNumber);
    void clearErrorHighlights();
    void highlightCurrentLine();
    void toggleBreakpoint(int line);
    void setExecutionLine(int lineNumber);
    void toggleBreakpointVisual(int line);

signals:
    void textStatusChanged(); // Signal to notify MainWindow of text/cursor changes and counts
    void breakpointToggled(int line);
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;   // ✅ For auto-completion key handling

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void insertCompletion(const QString &completion); // ✅ Slot for QCompleter

private:
    QWidget *lineNumberArea;
    QList<int> errorLines;
    QList<QTextEdit::ExtraSelection> _extraSelections;

    // Editor-level breakpoints for visual rendering
    QSet<int> breakpoints;

    // ✅ --- Auto Code Completion Members ---
    QCompleter *completer;
    QStringListModel *completerModel;
    QStringList keywordList;

    QString textUnderCursor() const;

    // ✅ --- Auto Save + Recovery Members ---
    QTimer *autoSaveTimer;
    QString autoSavePath;

    void autoSaveCode();
    void recoverLastSession();
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
