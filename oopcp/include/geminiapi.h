#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QList>

//
// --- MODIFIED ErrorInfo to match your new template ---
//
struct ErrorInfo {
    int lineNumber;
    QString errorType;
    QString description;
    QString fix; // Was 'suggestion', now holds the fixed code
};

// Define API constants
#define API_KEY "AIzaSyCWRz1t3H6O4ujg3zmeEc1JmQ51rjtY6Mk" // <-- IMPORTANT: Make sure this is correct
#define API_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent"

class GeminiAPI : public QObject
{
    Q_OBJECT

public:
    explicit GeminiAPI(QObject *parent = nullptr);

public slots:
    void analyzeCode(const QString &code);
    void explainCode(const QString &code);
    void analyzeComplexity(const QString &prompt);
    void explainDebugStep(const QString &prompt);
    void requestRefactor(const QString &prompt);


    void sendChatMessage(const QStringList &history, const QString &newMessage);


signals:
    void analysisComplete(const QList<ErrorInfo> &errors);
    void analysisError(const QString &errorMessage);
    void explanationComplete(const QString &explanation);
    void complexityAnalysisComplete(const QString &response);
    void debugExplanationReady(const QString &explanation);
    void refactorComplete(const QString &refactoredCode);


   
void chatResponseComplete(const QString &response);
void chatResponseError(const QString &errorMessage);


private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *networkManager;
    QList<ErrorInfo> parseResponse(const QString &response);
};