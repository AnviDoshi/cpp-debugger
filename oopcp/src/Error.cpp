#include "../include/Error.h"
#include <iostream>

namespace AIDebug {

void ErrorReport::print() const {
    std::cout << "--- Error Report ---\n";
    std::cout << "Line: " << line << "\n";
    std::cout << "Instruction: " << instruction << "\n";
    std::cout << "Kind: ";
    switch(kind){
        case ErrorKind::NONE: std::cout<<"None\n"; break;
        case ErrorKind::NULL_POINTER: std::cout<<"Null Pointer Dereference\n"; break;
        case ErrorKind::ARRAY_OOB: std::cout<<"Array Out of Bounds\n"; break;
        case ErrorKind::USE_AFTER_FREE: std::cout<<"Use After Free\n"; break;
        case ErrorKind::MEMORY_LEAK: std::cout<<"Memory Leak\n"; break;
        default: std::cout<<"Unknown\n"; break;
    }
    std::cout << "Message: " << message << "\n";
    if(address!=-1) std::cout << "Address: 0x" << std::hex << address << std::dec << "\n";
    std::cout << "---------------------\n";
}

} // ns
