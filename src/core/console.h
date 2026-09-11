#pragma once
// ---------------------------------------------------------------------------
// 平台抽象层：把控制台相关的平台差异（清屏、读取按键、编码初始化）集中到此处，
// 业务代码只依赖本头文件，避免在 combatSystem 中直接使用 conio/windows API。
//   - Windows：使用 conio.h / windows.h
//   - 其他平台：退化为 ANSI 清屏与标准输入
// ---------------------------------------------------------------------------
namespace console {
    // 扩展键码（非 ASCII，方向键等，Linux 下由转义序列解析而来）
    enum {
        KEY_UP    = 1000,
        KEY_DOWN  = 1001,
        KEY_LEFT  = 1002,
        KEY_RIGHT = 1003,
    };

    void init();        // 初始化（如设置控制台 UTF-8 编码）
    void clearScreen(); // 清屏
    int  readKey();     // 读取一个按键（无回显）
    void pause();       // 等待任意按键
    bool pauseEsc();    // 等待按键，若按下 ESC 返回 true
    void setColor(int colorCode); // 设置文字颜色（0~15，Windows 控制台色码）
    void sleep(int ms); // 延时（毫秒）
    bool kbhit();       // 非阻塞检测是否有按键
    void moveCursor(int row, int col); // 移动光标到 (row, col)，0 起始（用于覆盖重绘防闪烁）
    void setCursorVisible(bool visible); // 显示/隐藏光标（渲染时隐藏，防光标乱闪）
    void clearToEnd(); // 清光标到屏幕底（覆盖重绘后清掉尾部残留）
}
