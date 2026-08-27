#include "console.h"

#if defined(_WIN32)
#include <conio.h>
#include <windows.h>
#include <cstdlib>

namespace console {
    void init() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
    void clearScreen() { std::system("cls"); }
    int readKey() { return _getch(); }
    void pause() { _getch(); }
}
#else
#include <cstdio>
#include <cstdlib>

namespace console {
    void init() {}
    void clearScreen() { std::system("clear"); }
    int readKey() { return std::getchar(); }
    void pause() { std::getchar(); }
}
#endif
