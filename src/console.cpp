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
    bool pauseEsc() { return _getch() == 27; } // ESC 的键码为 27
    void setColor(int colorCode) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(h, static_cast<WORD>(colorCode));
    }
    void sleep(int ms) { Sleep(ms); }
    bool kbhit() { return _kbhit() != 0; }
}
#else
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>

namespace console {
    void init() {}
    void clearScreen() { std::system("clear"); }
    int readKey() { return std::getchar(); }
    void pause() { std::getchar(); }
    bool pauseEsc() { return std::getchar() == 27; } // ESC 的键码为 27
    void setColor(int colorCode) {
        // Windows 色码 → ANSI 前景色（0~15 → 30~37 / 90~97）
        static const char* fg[] = {
            "30","34","32","36","31","35","33","37",
            "90","94","92","96","91","95","93","97"
        };
        if (colorCode >= 0 && colorCode <= 15) std::printf("\033[%sm", fg[colorCode]);
    }
    void sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
    bool kbhit() { return false; } // 非 Windows 下暂不实现非阻塞检测
}
#endif
