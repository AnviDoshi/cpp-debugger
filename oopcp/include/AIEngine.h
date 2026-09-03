#ifndef AIENGINE_H
#define AIENGINE_H

#include "Error.h"
#include <string>

namespace AIDebug {

class AIEngine {
public:
    AIEngine();
    ~AIEngine();

    // analyze a raw ErrorReport and produce a human-friendly explanation
    std::string analyze(const ErrorReport &r);
};

}

#endif // AIENGINE_H
