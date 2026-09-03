#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>

namespace AIDebug {

inline std::string trim(const std::string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a==std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}

template<typename T>
std::string toString(const T &v){
    std::ostringstream os; os<<v; return os.str();
}

} // ns

#endif // UTILS_H
