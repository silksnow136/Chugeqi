#pragma once
#include <string>
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
    void setColor(int colorCode); // 设置文字颜色（0~15，Windows 控制台色码）
    void sleep(int ms); // 延时（毫秒）
    bool kbhit();       // 非阻塞检测是否有按键
    void setCursorVisible(bool visible); // 显示/隐藏光标（渲染时隐藏，防光标乱闪）
    void clearToEnd(); // 清光标到屏幕底（覆盖重绘后清掉尾部残留）

    // 计算 UTF-8 字符串的控制台显示宽度：
    //   ASCII 1 列，箭头(0x2190~0x2193)与制表符(0x2500~0x259F) 1 列，
    //   其余 CJK/全角字符 2 列。用于退格、补空格、按宽度换行等光标操作，
    //   避免用字节数导致光标错位。
    int displayWidth(const std::string& s);

    // 终端模式切换（Linux 有效，Windows 空实现）
    // 语义：readKey/kbhit 会进入 raw 模式；若调用前已是 raw（如 printWords 主动
    // 持有 raw），则读完后不恢复，交由外层统一恢复，从而在整个输出期间吞掉按键。
    void enterRaw();   // 进入原始模式：单键即时读取、关闭回显
    void drainInput(); // 丢弃所有待读按键并恢复 canonical（用于播放结束后清残留）

    // 读取一行命令：回显 + 退格 + 回车结束 + ESC 取消（返回空串）
    std::string readLine();
}
