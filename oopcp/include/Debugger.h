#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "AssemblyParser.h"
#include "Error.h"
#include <string>
#include <vector>

namespace AIDebug {

// A very simple debugger simulation: steps through AsmLine and simulates memory/register state
class Debugger {
public:
    Debugger();
    ~Debugger();

    // run a parsed assembly trace and return an ErrorReport (if any)
    ErrorReport runTrace(const std::vector<AsmLine> &trace);

private:
    // simulated memory bookkeeping (addresses allocated and freed)
    std::vector<int> allocations; // addresses
    bool freedRecently = false;

    // helpers
    int parseAddress(const std::string &op); // parses something like [0x0], 0x1000 or 0
};

}

#endif // DEBUGGER_H
