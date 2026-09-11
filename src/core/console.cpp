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
    void moveCursor(int row, int col) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = { static_cast<SHORT>(col), static_cast<SHORT>(row) };
        SetConsoleCursorPosition(h, pos);
    }
    void setCursorVisible(bool visible) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        GetConsoleCursorInfo(h, &info);
        info.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(h, &info);
    }
    void clearToEnd() {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(h, &csbi)) return;
        DWORD written;
        int cells = (csbi.dwSize.Y - csbi.dwCursorPosition.Y) * csbi.dwSize.X - csbi.dwCursorPosition.X;
        if (cells > 0) {
            FillConsoleOutputCharacter(h, ' ', cells, csbi.dwCursorPosition, &written);
            FillConsoleOutputAttribute(h, csbi.wAttributes, cells, csbi.dwCursorPosition, &written);
        }
    }
}
#else
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

namespace {

    termios g_original{};
    bool g_saved = false;
    bool g_raw = false;

    void restoreTerm();

    // 进入原始模式：关闭行缓冲(ICANON)与回显(ECHO)，逐字节即时读取
    void ensureRaw() {
        if (g_raw) return;
        if (tcgetattr(STDIN_FILENO, &g_original) != 0) return;
        g_saved = true;
        termios raw = g_original;
        raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
        raw.c_iflag &= ~static_cast<tcflag_t>(ICRNL | IXON);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
            g_raw = true;
            std::atexit(restoreTerm);
        }
    }

    void restoreTerm() {
        if (g_saved) tcsetattr(STDIN_FILENO, TCSANOW, &g_original);
        g_raw = false;
    }

    // 非阻塞检测 stdin 是否有可读字节
    bool pollIn(int timeoutMs) {
        pollfd pfd{STDIN_FILENO, POLLIN, 0};
        return poll(&pfd, 1, timeoutMs) > 0;
    }
}

namespace console {
    void init() { ensureRaw(); }
    void clearScreen() { std::system("clear"); }

    int readKey() {
        ensureRaw();
        int c = std::getchar();
        if (c == EOF) return c;
        if (c != 27) return c;          // 普通按键
        // ESC：可能是裸 ESC，也可能是方向键转义序列 \033[A/B/C/D
        if (!pollIn(20)) return 27;
        int c2 = std::getchar();
        if (c2 == EOF || c2 != '[') return 27;
        if (!pollIn(20)) return 27;
        int c3 = std::getchar();
        switch (c3) {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            default:  return 27;
        }
    }

    void pause() { readKey(); }
    bool pauseEsc() { return readKey() == 27; }

    void setColor(int colorCode) {
        // Windows 色码 → ANSI 前景色（0~15 → 30~37 / 90~97）
        static const char* fg[] = {
            "30","34","32","36","31","35","33","37",
            "90","94","92","96","91","95","93","97"
        };
        if (colorCode >= 0 && colorCode <= 15) std::printf("\033[%sm", fg[colorCode]);
    }
    void sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
    bool kbhit() { ensureRaw(); return pollIn(0); }
    void moveCursor(int row, int col) { std::printf("\033[%d;%dH", row + 1, col + 1); }
    void setCursorVisible(bool visible) { std::printf(visible ? "\033[?25h" : "\033[?25l"); }
    void clearToEnd() { std::printf("\033[J"); }
}
#endif
