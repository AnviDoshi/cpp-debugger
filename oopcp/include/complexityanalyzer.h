#ifndef COMPLEXITYANALYZER_H
#define COMPLEXITYANALYZER_H

#include <QString>
#include <QList>

struct ComplexityResult {
    QString timeComplexity;   // e.g. "O(n^2) (estimated)"
    QString spaceComplexity;  // e.g. "O(1) (estimated)"
    QString reason;           // short deterministic reason from heuristics
    QString promptForAI;      // the final prompt we send to the AI
};

class ComplexityAnalyzer
{
public:
    ComplexityAnalyzer();
    ComplexityResult analyze(const QString &code);

private:
    int countForLoops(const QString &code);
    int countWhileLoops(const QString &code);
    bool hasNestedLoops(const QString &code);
    bool hasRecursion(const QString &code);
    bool containsExpensiveOps(const QString &code); // sort, regex, io etc
    QString guessTimeByHeuristics(const QString &code);
    QString guessSpaceByHeuristics(const QString &code);
    QString buildAIPrompt(const QString &code, const ComplexityResult &heur);
};

#endif // COMPLEXITYANALYZER_H
