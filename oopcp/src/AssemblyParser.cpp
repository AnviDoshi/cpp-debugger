#include "../include/AssemblyParser.h"
#include "../include/Utils.h"
#include <fstream>
#include <sstream>

namespace AIDebug {

using namespace std;

static std::vector<std::string> splitOps(const std::string &s){
    vector<string> out;
    string cur; stringstream ss(s);
    while(getline(ss, cur, ',')){
        out.push_back(trim(cur));
    }
    return out;
}

std::vector<AsmLine> AssemblyParser::parseFile(const std::string &path){
    vector<AsmLine> lines;
    ifstream fin(path);
    if(!fin.is_open()) return lines;
    string raw; int lineno = 0;
    while(getline(fin, raw)){
        lineno++;
        string t = trim(raw);
        if(t.empty()) continue;
        // naive parse: instr is first token
        string instr;
        string ops;
        stringstream ss(t);
        ss >> instr;
        getline(ss, ops);
        AsmLine a;
        a.lineNo = lineno;
        a.text = t;
        a.instr = instr;
        a.ops = splitOps(ops);
        lines.push_back(a);
    }
    return lines;
}

}
