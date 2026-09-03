#include <iostream>
#include "../include/AssemblyParser.h"
#include "../include/Debugger.h"
#include "../include/AIEngine.h"

using namespace AIDebug;
int main(int argc, char **argv){
    if(argc<2){
        std::cout << "Usage: ai_debugger <assembly_trace_file>\n";
        return 1;
    }
    std::string path = argv[1];
    auto trace = AssemblyParser::parseFile(path);
    if(trace.empty()){
        std::cout << "No instructions parsed from file: "<< path << "\n";
        return 1;
    }

    Debugger dbg;
    ErrorReport rpt = dbg.runTrace(trace);

    AIEngine ai;
    std::string explanation = ai.analyze(rpt);

    // print results
    if(rpt.kind != ErrorKind::NONE){
        rpt.print();
    }
    std::cout << "AI Explanation:\n" << explanation << "\n";

    return 0;
}
