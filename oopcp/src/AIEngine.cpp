#include "../include/AIEngine.h"
#include <map>

namespace AIDebug {

AIEngine::AIEngine() {}
AIEngine::~AIEngine() {}

std::string AIEngine::analyze(const ErrorReport &r){
    if(r.kind == ErrorKind::NONE) return "No error detected by rule-based engine.";
    std::map<ErrorKind, std::string> explanations;
    explanations[ErrorKind::NULL_POINTER] =
        "Segmentation fault likely caused by a null pointer dereference. Check pointers used at the reported line.\nSuggestion: initialize the pointer or check for null before dereferencing.";
    explanations[ErrorKind::ARRAY_OOB] =
        "Array index out of bounds detected. Verify loop limits and array sizes.\nSuggestion: add boundary checks or fix index computations.";
    explanations[ErrorKind::USE_AFTER_FREE] =
        "Use-after-free detected: memory was accessed after being freed.\nSuggestion: avoid using pointers after free, or set pointer to null after free.";
    explanations[ErrorKind::MEMORY_LEAK] =
        "Memory leak detected: allocated memory was not freed before program exit.\nSuggestion: ensure every allocation has a matching deallocation (delete/free).";
    explanations[ErrorKind::UNKNOWN] = "Unknown error.";

    auto it = explanations.find(r.kind);
    if(it!=explanations.end()){
        std::string out = it->second;
        out += "\n(Assembly instr: "+ r.instruction + ")";
        return out;
    }
    return "No explanation available.";
}

}
