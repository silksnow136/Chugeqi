#include "console.h"

#if defined(_WIN32)
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <string>

namespace console {
    void init() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
    void clearScreen() {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(h, &csbi)) return;
        // 清空整个屏幕缓冲区（含滚动历史），并复位光标到左上角
        DWORD cells = static_cast<DWORD>(csbi.dwSize.X) * csbi.dwSize.Y;
        COORD origin = { 0, 0 };
        DWORD written = 0;
        FillConsoleOutputCharacter(h, ' ', cells, origin, &written);
        FillConsoleOutputAttribute(h, csbi.wAttributes, cells, origin, &written);
        SetConsoleCursorPosition(h, origin);
    }
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

    // Windows 控制台天然支持 _getch 单键读取，无需切换终端模式
    void enterRaw() {}
    void restoreCanonical() {}
    void drainInput() {}

    std::string readLine() {
        std::string cmd;
        while (true) {
            int k = readKey();
            if (k == 13 || k == 10) { std::printf("\n"); break; }   // 回车
            if (k == 27) { std::printf("\n"); return ""; }          // ESC 取消
            if (k == 8) {
                if (!cmd.empty()) { cmd.pop_back(); std::printf("\b \b"); } // 退格
            } else if (k >= 32 && k < 127) {
                cmd += static_cast<char>(k);
                std::putchar(k);
            }
        }
        return cmd;
    }
}
#else
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

namespace {

    termios g_original{};
    bool g_saved = false;
    bool g_raw = false;

    void restoreTerm(); // atexit 兜底恢复

    // 首次调用时保存原始终端属性（供 restoreCanonical 恢复）
    void saveOriginal() {
        if (g_saved) return;
        if (tcgetattr(STDIN_FILENO, &g_original) != 0) return;
        g_saved = true;
        std::atexit(restoreTerm);
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
    void init() { saveOriginal(); } // 仅保存原始属性，不再常驻 raw 模式

    // 快速清屏（直接 ANSI，避免 system() 子进程导致的闪烁）：
    // \033[H 光标复位，\033[2J 清屏，\033[3J 清滚动历史（防止旧地图残留）
    void clearScreen() { std::printf("\033[H\033[2J\033[3J"); }

    // 进入原始模式：关闭行缓冲(ICANON)与回显(ECHO)，逐字节即时读取
    void enterRaw() {
        saveOriginal();
        if (g_raw) return;
        termios raw = g_original;
        raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
        raw.c_iflag &= ~static_cast<tcflag_t>(ICRNL | IXON);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) g_raw = true;
    }

    // 恢复行缓冲模式：让 getline / cin >> 等行输入正常工作
    void restoreCanonical() {
        if (!g_raw) return;
        if (g_saved) tcsetattr(STDIN_FILENO, TCSANOW, &g_original);
        g_raw = false;
    }

    int readKey() {
        bool wasRaw = g_raw;   // 记录是否已处于 raw：若是，读完不恢复，交由外层统一恢复
        enterRaw();
        int c = std::getchar();
        if (c == EOF) { if (!wasRaw) restoreCanonical(); return c; }
        if (c != 27) { if (!wasRaw) restoreCanonical(); return c; } // 普通按键
        // ESC：可能是裸 ESC，也可能是方向键转义序列 \033[A/B/C/D
        if (!pollIn(20)) { if (!wasRaw) restoreCanonical(); return 27; }
        int c2 = std::getchar();
        if (c2 == EOF || c2 != '[') { if (!wasRaw) restoreCanonical(); return 27; }
        if (!pollIn(20)) { if (!wasRaw) restoreCanonical(); return 27; }
        int c3 = std::getchar();
        if (!wasRaw) restoreCanonical();
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

    bool kbhit() {
        bool wasRaw = g_raw;   // 若已持有 raw，查完不恢复
        enterRaw();
        bool has = pollIn(0);
        if (!wasRaw) restoreCanonical();
        return has;
    }

    // 丢弃所有待读按键并恢复 canonical：用于剧情播放等「输出期间吞键」的场景，
    // 确保返回命令输入前终端干净（无残留字符、回显恢复）。
    void drainInput() {
        enterRaw();
        while (pollIn(0)) {
            if (std::getchar() == EOF) break;
        }
        restoreCanonical();
    }

    void moveCursor(int row, int col) { std::printf("\033[%d;%dH", row + 1, col + 1); }
    void setCursorVisible(bool visible) { std::printf(visible ? "\033[?25h" : "\033[?25l"); }
    void clearToEnd() { std::printf("\033[J"); }

    // 读取一行命令：回显 + 退格 + 回车结束 + ESC 取消（返回空串）
    std::string readLine() {
        std::string cmd;
        while (true) {
            int k = readKey();
            if (k == 13 || k == 10) { std::cout << '\n' << std::flush; break; }   // 回车
            if (k == 27) { std::cout << '\n' << std::flush; return ""; }          // ESC 取消
            if (k == 8 || k == 127) {
                if (!cmd.empty()) { cmd.pop_back(); std::cout << "\b \b" << std::flush; } // 退格
            } else if (k >= 32 && k < 127) {
                cmd += static_cast<char>(k);
                std::cout << static_cast<char>(k) << std::flush;
            }
        }
        return cmd;
    }
}
#endif
