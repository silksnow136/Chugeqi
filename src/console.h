#pragma once
// ---------------------------------------------------------------------------
// 平台抽象层：把控制台相关的平台差异（清屏、读取按键、编码初始化）集中到此处，
// 业务代码只依赖本头文件，避免在 combatSystem 中直接使用 conio/windows API。
//   - Windows：使用 conio.h / windows.h
//   - 其他平台：退化为 ANSI 清屏与标准输入
// ---------------------------------------------------------------------------
namespace console {
    void init();        // 初始化（如设置控制台 UTF-8 编码）
    void clearScreen(); // 清屏
    int  readKey();     // 读取一个按键（无回显）
    void pause();       // 等待任意按键
    bool pauseEsc();    // 等待按键，若按下 ESC 返回 true
}
