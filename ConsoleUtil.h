#pragma once
#include <cstdlib>

inline void setupConsoleUTF8() {
#ifdef _WIN32
    std::system("chcp 65001 > nul");
#endif
}
