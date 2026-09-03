#include "complexityanalyzer.h"
#include <QRegularExpression>

ComplexityAnalyzer::ComplexityAnalyzer() {}

static QString norm(const QString &s) {
    // minimal cleanup
    QString r = s;
    r.replace("\r\n", "\n");
    r.replace("\t", " ");
    return r;
}

int ComplexityAnalyzer::countForLoops(const QString &code) {
    QRegularExpression re("\\bfor\\s*\\(");
    return code.count(re);
}

int ComplexityAnalyzer::countWhileLoops(const QString &code) {
    QRegularExpression re("\\bwhile\\s*\\(");
    return code.count(re);
}

bool ComplexityAnalyzer::hasNestedLoops(const QString &code) {
    // Only detect classic patterns of a for within another for or while within another while, on consecutive lines.
    QRegularExpression simpleNestedFor("\\bfor\\s*\\([^)]*\\)\\s*\\{[^\\{\\}]*\\bfor\\s*\\(", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression simpleNestedWhile("\\bwhile\\s*\\([^)]*\\)\\s*\\{[^\\{\\}]*\\bwhile\\s*\\(", QRegularExpression::DotMatchesEverythingOption);
    return simpleNestedFor.match(code).hasMatch() || simpleNestedWhile.match(code).hasMatch();
}


bool ComplexityAnalyzer::hasRecursion(const QString &code) {
    // Match function header
    QRegularExpression funcRe("([A-Za-z_][A-Za-z0-9_]*)\\s*\\([^\\)]*\\)\\s*\\{");
    auto it = funcRe.globalMatch(code);
    while (it.hasNext()) {
        auto m = it.next();
        QString fname = m.captured(1);
        int start = m.capturedEnd(0);
        int end = code.indexOf('}', start);
        if (end == -1) continue;
        QString body = code.mid(start, end - start);
        QRegularExpression callRe("\\b" + QRegularExpression::escape(fname) + "\\s*\\(");
        if (callRe.match(body).hasMatch()) return true;
    }
    return false;
}

bool ComplexityAnalyzer::containsExpensiveOps(const QString &code) {
    // look for sort, stable_sort, push_back in nested loops, std::regex, std::sort, getline/printf/cout inside loops
    if (code.contains("std::sort") || code.contains("sort(") || code.contains("std::regex")) return true;
    // basic: cout/printf inside loops detection
    QRegularExpression loopWithIO("for\\s*\\([^\\)]*\\)\\s*\\{[^}]*\\b(cout|printf|std::cout)\\b",
                                  QRegularExpression::DotMatchesEverythingOption);
    return loopWithIO.match(code).hasMatch();
}

QString ComplexityAnalyzer::guessTimeByHeuristics(const QString &code) {
    int f = countForLoops(code);
    int w = countWhileLoops(code);
    bool nested = hasNestedLoops(code);

    QRegularExpression binSearchRe("\\bbinary_search\\s*\\(");
    if (binSearchRe.match(code).hasMatch())
        return "O(log n) (binary search detected)";

    QRegularExpression sortRe("\\b(sort|std::sort)\\s*\\(");
    if (sortRe.match(code).hasMatch())
        return "O(n log n) (sort detected)";

    // O(n!) for permutation-recursive pattern, as previously
    QRegularExpression permRe("void\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*\\([^\\)]*\\)\\s*\\{[^}]*for\\s*\\([^)]*\\)[^}]*\\1\\(",
        QRegularExpression::DotMatchesEverythingOption);
    if (permRe.match(code).hasMatch())
        return "O(n!) (recursive permutation detected)";

    bool rec = hasRecursion(code);
    //bool expensive = containsExpensiveOps(code);

    if (nested) return "O(n^2) (nested loops detected)";
    if (rec) return "O(2^n) (recursion detected)";
    if (f + w > 1) return "O(n*k) (multiple loops)";
    if (f + w == 1) return "O(n) (single loop)";
    return "O(1) (no loops/recursion)";
}

QString ComplexityAnalyzer::guessSpaceByHeuristics(const QString &code) {
    // simple: look for containers and allocations
    if (code.contains("new ") || code.contains("std::vector") || code.contains("std::string") || code.contains("malloc("))
        return "Possibly O(n) (allocations/containers detected)";
    return "O(1) (no obvious allocations)";
}

QString ComplexityAnalyzer::buildAIPrompt(const QString &code, const ComplexityResult &heur) {
    QString prompt;
    prompt += "You are an expert C++ performance analyst. Analyze the code below and provide:\n";
    prompt += "1) Best estimate of Time Complexity (explain assumptions precisely).\n";
    prompt += "2) Best estimate of Space Complexity (explain assumptions).\n";
    prompt += "3) Which lines/statements are the bottleneck and why.\n";
    prompt += "4) Practical optimizations (prefer C++ idiomatic changes), and show a short improved snippet if applicable.\n";
    prompt += "5) If exact complexity cannot be determined, explain what runtime measurements or static info are required.\n\n";

    prompt += "Heuristic summary (from fast local detection):\n";
    prompt += " - Heuristic Time: " + heur.timeComplexity + "\n";
    prompt += " - Heuristic Space: " + heur.spaceComplexity + "\n";
    prompt += " - Heuristic reason: " + heur.reason + "\n\n";

    prompt += "Code:\n```cpp\n";
    prompt += code + "\n```\n";
    prompt += "Answer concisely but clearly, with bullet points and a short example where useful.";
    return prompt;
}

ComplexityResult ComplexityAnalyzer::analyze(const QString &code) {
    ComplexityResult r;
    QString cleaned = norm(code);
    r.timeComplexity = guessTimeByHeuristics(cleaned);
    r.spaceComplexity = guessSpaceByHeuristics(cleaned);
    r.reason = QString("for=%1, while=%2, nested=%3, recursion=%4, expensiveOps=%5")
                   .arg(countForLoops(cleaned))
                   .arg(countWhileLoops(cleaned))
                   .arg(hasNestedLoops(cleaned) ? "yes" : "no")
                   .arg(hasRecursion(cleaned) ? "yes" : "no")
                   .arg(containsExpensiveOps(cleaned) ? "yes" : "no");

    r.promptForAI = buildAIPrompt(cleaned, r);
    return r;
}
