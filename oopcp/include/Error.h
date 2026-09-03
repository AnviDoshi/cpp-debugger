#ifndef ERROR_H
#define ERROR_H

#include <string>

namespace AIDebug {

enum class ErrorKind { NONE, NULL_POINTER, ARRAY_OOB, USE_AFTER_FREE, MEMORY_LEAK, UNKNOWN };

struct ErrorReport {
    ErrorKind kind = ErrorKind::NONE;
    std::string message;
    std::string instruction; // assembly instruction where error occurred
    int address = -1;        // memory address (simulated)
    int line = -1;           // line in the assembly trace

    void print() const;
};

} // ns

#endif // ERROR_H
