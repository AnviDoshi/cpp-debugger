#include "../include/Debugger.h"
#include "../include/Utils.h"
#include <sstream>
#include <string>
using std::string;

namespace AIDebug {

Debugger::Debugger() {}
Debugger::~Debugger() {}

int Debugger::parseAddress(const std::string &op){
    // naive: if op contains '0x' parse hex; if number parse decimal; if '0' return 0
    string s = op;
    // remove brackets and non-hex chars
    for(char &c: s) if(c=='[' || c==']') c=' ';
    s = trim(s);
    if(s.empty()) return -1;
    try{
        if(s.rfind("0x",0)==0) {
            int v = std::stoi(s, nullptr, 16);
            return v;
        }
        return std::stoi(s);
    } catch(...) { return -1; }
}

ErrorReport Debugger::runTrace(const std::vector<AsmLine> &trace){
    ErrorReport rpt;
    rpt.kind = ErrorKind::NONE;
    for(const auto &line : trace){
        // sample rule checks while stepping
        if(line.instr == "MOV" || line.instr=="MOVD" || line.instr=="MOVM"){
            // check operand like [0x0]
            if(!line.ops.empty()){
                int addr = parseAddress(line.ops[0]);
                if(addr==0){
                    rpt.kind = ErrorKind::NULL_POINTER;
                    rpt.instruction = line.text;
                    rpt.line = line.lineNo;
                    rpt.address = addr;
                    rpt.message = "Detected write/read to address 0x0, likely null pointer dereference.";
                    return rpt;
                }
                // if freedRecently and addr in allocations => use-after-free
                if(freedRecently){
                    for(int a: allocations){
                        if(a==addr){
                            rpt.kind = ErrorKind::USE_AFTER_FREE;
                            rpt.instruction = line.text;
                            rpt.line = line.lineNo;
                            rpt.address = addr;
                            rpt.message = "Access to recently freed memory (use-after-free).";
                            return rpt;
                        }
                    }
                }
            }
        }
        else if(line.instr == "ALLOC"){
            // e.g., ALLOC 0x1000
            if(!line.ops.empty()){
                int addr = parseAddress(line.ops[0]);
                if(addr!=-1) allocations.push_back(addr);
                freedRecently = false;
            }
        }
        else if(line.instr == "FREE"){
            if(!line.ops.empty()){
                int addr = parseAddress(line.ops[0]);
                // mark as freed
                for(auto it = allocations.begin(); it!=allocations.end(); ++it){
                    if(*it==addr){ allocations.erase(it); freedRecently = true; break; }
                }
            }
        }
        else if(line.instr == "CHECK_ARRAY"){
            // CHECK_ARRAY idx, size  (example custom instr in trace)
            if(line.ops.size()>=2){
                int idx = parseAddress(line.ops[0]);
                int size = parseAddress(line.ops[1]);
                if(idx>=size){
                    rpt.kind = ErrorKind::ARRAY_OOB;
                    rpt.instruction = line.text;
                    rpt.line = line.lineNo;
                    rpt.message = "Index >= size: array out-of-bounds access.";
                    return rpt;
                }
            }
        }
    }

    // after trace, if allocations leftover => memory leak
    if(!allocations.empty()){
        rpt.kind = ErrorKind::MEMORY_LEAK;
        rpt.message = "Allocated memory not freed before program exit.";
        rpt.instruction = "<end-of-trace>";
        rpt.line = trace.empty()?0:trace.back().lineNo;
        return rpt;
    }

    return rpt; // NONE
}

}
