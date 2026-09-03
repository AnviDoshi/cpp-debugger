#ifndef ASSEMBLYPARSER_H
#define ASSEMBLYPARSER_H

#include <string>
#include <vector>

namespace AIDebug {

struct AsmLine {
    int lineNo;
    std::string text;    // raw line
    std::string instr;   // instruction mnemonic
    std::vector<std::string> ops; // operands
};

class AssemblyParser {
public:
    // parse a simplified assembly trace file into AsmLine items
    static std::vector<AsmLine> parseFile(const std::string &path);
};

}

#endif // ASSEMBLYPARSER_H
