#include "../include/geminiapi.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QRegularExpression> // Needed for parsing


#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

GeminiAPI::GeminiAPI(QObject *parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void GeminiAPI::analyzeCode(const QString &code)
{
    QString url = QString(API_URL) + "?key=" + API_KEY;
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    //
    // --- THIS IS THE NEW, STRICTER PROMPT ---
    //
    QString prompt = "You are a meticulous C++ code analyzer. You must act like a strict compiler *first*, then a logical reviewer.\n"
                     "Analyze the following code. You MUST find all errors, *especially* simple syntax errors like missing semicolons.\n"
                     "Focus on:\n"
                     "1. **Syntax Errors:** Missing ';', '}', '()', type mismatches, etc.\n"
                     "2. **Logical Errors:** Infinite loops, incorrect variable usage, flawed logic.\n"
                     "3. **Best Practices:** Uninitialized variables, potential issues.\n\n"
                     "For EACH error found, respond in this EXACT format. Do not skip any part:\n"
                     "ERROR_START\n"
                     "Line: <line_number>\n"
                     "Type: <Syntax|Logic|Best Practice>\n"
                     "Description: <brief, 1-sentence description of the problem>\n"
                     "Fix: <The single, corrected line of C++ code. Do not add explanation, just the code.>\n"
                     "ERROR_END\n\n"
                     "Code to analyze:\n```cpp\n" + code + "\n```\n\n"
                     "If no errors are found, respond with: NO_ERRORS_FOUND";
    //
    // --- END NEW PROMPT ---
    //
    
    QJsonObject contentObj;
    QJsonArray partsArray;
    QJsonObject textObj;
    textObj["text"] = prompt;
    partsArray.append(textObj);
    contentObj["parts"] = partsArray;
    
    QJsonArray contentsArray;
    contentsArray.append(contentObj);
    
    QJsonObject requestBody;
    requestBody["contents"] = contentsArray;
    
    QJsonDocument doc(requestBody);
    QNetworkReply *reply = networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &GeminiAPI::onReplyFinished);
}

void GeminiAPI::explainCode(const QString &code)
{
    QNetworkRequest request(QUrl(QString(API_URL) + "?key=" + API_KEY));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    QJsonArray contents;
    QJsonObject partsObj;
    QJsonArray parts;

    parts.append(QJsonObject{{"text", "Explain the following C++ code in simple terms, line by line, focusing on logic and purpose:\n" + code}});
    partsObj["parts"] = parts;
    contents.append(partsObj);
    payload["contents"] = contents;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit analysisError("AI Explanation Error: " + reply->errorString());
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString explanation;

        if (jsonDoc["candidates"].isArray()) {
            QJsonArray candidates = jsonDoc["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject first = candidates[0].toObject();
                if (first.contains("content")) {
                    QJsonObject content = first["content"].toObject();
                    if (content.contains("parts")) {
                        QJsonArray parts = content["parts"].toArray();
                        for (const auto &part : parts)
                            explanation += part.toObject().value("text").toString() + "\n";
                    }
                }
            }
        }

        if (explanation.isEmpty())
            explanation = " Gemini could not generate an explanation for this code.";

        emit explanationComplete(explanation.trimmed());
    });
}

void GeminiAPI::analyzeComplexity(const QString &prompt)
{
    QNetworkRequest request(QUrl(QString(API_URL) + "?key=" + API_KEY));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    QJsonArray contents;
    QJsonObject partsObj;
    QJsonArray parts;

    parts.append(QJsonObject{{"text", prompt}});
    partsObj["parts"] = parts;
    contents.append(partsObj);
    payload["contents"] = contents;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit analysisError("AI Complexity Error: " + reply->errorString());
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString explanation;

        if (jsonDoc["candidates"].isArray()) {
            QJsonArray candidates = jsonDoc["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject first = candidates[0].toObject();
                if (first.contains("content")) {
                    QJsonObject content = first["content"].toObject();
                    if (content.contains("parts")) {
                        QJsonArray parts = content["parts"].toArray();
                        for (const auto &part : parts)
                            explanation += part.toObject().value("text").toString() + "\n";
                    }
                }
            }
        }

        if (explanation.isEmpty())
            explanation = " Gemini could not generate a complexity analysis for this code.";

        emit complexityAnalysisComplete(explanation.trimmed());
    });
}

void GeminiAPI::explainDebugStep(const QString &prompt)
{
    QNetworkRequest request(QUrl(QString(API_URL) + "?key=" + API_KEY));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    QJsonArray contents;
    QJsonObject partsObj;
    QJsonArray parts;

    parts.append(QJsonObject{{"text", prompt}});
    partsObj["parts"] = parts;
    contents.append(partsObj);
    payload["contents"] = contents;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit debugExplanationReady("<b>AI Debug Error:</b> " + reply->errorString());
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString explanation;

        if (jsonDoc["candidates"].isArray()) {
            QJsonArray candidates = jsonDoc["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject first = candidates[0].toObject();
                if (first.contains("content")) {
                    QJsonObject content = first["content"].toObject();
                    if (content.contains("parts")) {
                        QJsonArray parts = content["parts"].toArray();
                        for (const auto &part : parts)
                            explanation += part.toObject().value("text").toString() + "\n";
                    }
                }
            }
        }

        if (explanation.isEmpty())
            explanation = "<b>AI Debug Error:</b> Gemini could not generate an explanation.";

        emit debugExplanationReady(explanation.trimmed());
    });
}


void GeminiAPI::requestRefactor(const QString &prompt)
{
    QNetworkRequest request(QUrl(QString(API_URL) + "?key=" + API_KEY));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    QJsonArray contents;
    QJsonObject partsObj;
    QJsonArray parts;

    parts.append(QJsonObject{{"text", prompt}});
    partsObj["parts"] = parts;
    contents.append(partsObj);
    payload["contents"] = contents;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit refactorComplete("// AI Error: " + reply->errorString());
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString fullText;

        if (jsonDoc["candidates"].isArray()) {
            QJsonArray candidates = jsonDoc["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject first = candidates[0].toObject();
                if (first.contains("content")) {
                    QJsonObject content = first["content"].toObject();
                    if (content.contains("parts")) {
                        QJsonArray parts = content["parts"].toArray();
                        for (const auto &part : parts)
                            fullText += part.toObject().value("text").toString() + "\n";
                    }
                }
            }
        }

        if (fullText.isEmpty()) {
            emit refactorComplete("// AI Error: Could not parse refactored code.");
            return;
        }

        // --- NEW PARSING LOGIC ---
        // Try to find a C++ code block
        QRegularExpression re("```cpp\\s*([\\s\\S]*?)\\s*```");
        QRegularExpressionMatch match = re.match(fullText);
        
        QString refactoredCode;
        if (match.hasMatch()) {
            refactoredCode = match.captured(1).trimmed(); // Get just the code
        } else {
            // Fallback: maybe the AI listened and sent only code
            // A simple check: does it look like code?
            if (fullText.contains("int main") || fullText.contains("#include") || fullText.contains(";") || fullText.contains("}")) {
                    refactoredCode = fullText;
            } else {
                // It's probably an explanation, not just code.
                refactoredCode = "// AI Error: Could not extract code block from response.\n\n" + fullText;
            }
        }
        // --- END PARSING LOGIC ---

        emit refactorComplete(refactoredCode.trimmed());
    });
}


void GeminiAPI::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    
    if (!reply) {
        emit analysisError("Network reply is null");
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        emit analysisError("Network error: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray response = reply->readAll();
    QString responseStr = QString::fromUtf8(response);
    
    qDebug() << "API Response:" << responseStr;
    
    // Parse JSON response
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        emit analysisError("Invalid JSON response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray candidates = jsonObj["candidates"].toArray();
    
    if (candidates.isEmpty()) {
        if (jsonObj.contains("promptFeedback")) {
            QString reason = jsonObj["promptFeedback"].toObject()["blockReason"].toString();
            emit analysisError("Request blocked by API: " + reason);
        } else {
            emit analysisError("No response from Gemini API");
        }
        reply->deleteLater();
        return;
    }
    
    QJsonObject candidate = candidates[0].toObject();
    QJsonObject content = candidate["content"].toObject();
    QJsonArray parts = content["parts"].toArray();
    
    if (parts.isEmpty()) {
        emit analysisError("Empty response from API");
        reply->deleteLater();
        return;
    }
    
    QString text = parts[0].toObject()["text"].toString();
    
    QList<ErrorInfo> errors = parseResponse(text);
    emit analysisComplete(errors);
    
    reply->deleteLater();
}

//
// --- UPDATED PARSER for new "Fix:" field ---
//
QList<ErrorInfo> GeminiAPI::parseResponse(const QString &response)
{
    QList<ErrorInfo> errors;
    
    if (response.contains("NO_ERRORS_FOUND")) {
        return errors; // Empty list
    }
    
    QStringList blocks = response.split("ERROR_START", Qt::SkipEmptyParts);
    
    for (const QString &block : blocks) {
        if (!block.contains("ERROR_END")) continue;
        
        QString errorBlock = block.split("ERROR_END")[0];
        QStringList lines = errorBlock.split("\n", Qt::SkipEmptyParts);
        
        ErrorInfo error;
        error.lineNumber = -1;
        
        QString currentField;
        QString buffer; // To hold multi-line fields

        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            
            if (trimmed.startsWith("Line:")) {
                currentField = "Line";
                QString lineNum = trimmed.mid(5).trimmed();
                bool ok;
                error.lineNumber = lineNum.toInt(&ok);
                if (!ok) error.lineNumber = -1;
            }
            else if (trimmed.startsWith("Type:")) {
                currentField = "Type";
                error.errorType = trimmed.mid(5).trimmed();
            }
            else if (trimmed.startsWith("Description:")) {
                currentField = "Description";
                error.description = trimmed.mid(12).trimmed();
            }
            else if (trimmed.startsWith("Fix:")) {
                currentField = "Fix";
                error.fix = trimmed.mid(4).trimmed(); // Get the fix
            }
            else {
                // This is a continuation of a multi-line field
                if (currentField == "Description") {
                    error.description += "\n" + trimmed;
                } else if (currentField == "Fix") {
                    error.fix += "\n" + trimmed; // Append if fix is multi-line
                }
            }
        }
        
        if (error.lineNumber > 0 && !error.errorType.isEmpty()) {
            errors.append(error);
        }
    }
    
    return errors;
}



void GeminiAPI::sendChatMessage(const QStringList &history, const QString &newMessage)
{
    QNetworkRequest request(QUrl(QString(API_URL) + "?key=" + API_KEY));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    QJsonArray contentsArray;

    // 1. Add the entire history
    for (const QString &item : history) {
        QString role = item.startsWith("USER: ") ? "user" : "model";
        QString text = item.mid(role.length() + 2); // +2 for ": "

        QJsonObject partObj;
        partObj["text"] = text;

        QJsonArray partsArray;
        partsArray.append(partObj);

        QJsonObject contentObj;
        contentObj["role"] = role;
        contentObj["parts"] = partsArray;

        contentsArray.append(contentObj);
    }

    // 2. Add the new user message
    QJsonObject partObj;
    partObj["text"] = newMessage;

    QJsonArray partsArray;
    partsArray.append(partObj);

    QJsonObject contentObj;
    contentObj["role"] = "user";
    contentObj["parts"] = partsArray;

    contentsArray.append(contentObj);
    
    // 3. Set the full conversation as the payload
    payload["contents"] = contentsArray;

    // 4. Send the request
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());

    // 5. Connect the reply to a new lambda function
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit chatResponseError("AI Chat Error: " + reply->errorString());
            return;
        }

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QString textResponse;

        // --- Standard Gemini JSON parsing ---
        if (jsonDoc["candidates"].isArray()) {
            QJsonArray candidates = jsonDoc["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject first = candidates[0].toObject();
                if (first.contains("content")) {
                    QJsonObject content = first["content"].toObject();
                    if (content.contains("parts")) {
                        QJsonArray parts = content["parts"].toArray();
                        if (!parts.isEmpty()) {
                            textResponse = parts[0].toObject().value("text").toString();
                        }
                    }
                }
            }
        }
        // --- End parsing ---

        if (textResponse.isEmpty()) {
            // Check for a block reason
            if (jsonDoc.object().contains("promptFeedback")) {
                QString reason = jsonDoc.object()["promptFeedback"].toObject()["blockReason"].toString();
                emit chatResponseError("Request blocked: " + reason);
            } else {
                emit chatResponseError("AI Chat Error: Received an empty response.");
            }
            return;
        }

        emit chatResponseComplete(textResponse.trimmed());
    });
}