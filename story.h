#pragma once
#include <string>
#include <vector>

// 一行叙事（旁白或对话），由 data/story.json 载入
struct StoryLine {
    std::string text;      // 显示文本（可含 \n）
    int color = 14;        // 文字颜色（Windows 控制台色码）
    int sleep = 0;         // 播完后停顿毫秒
    bool wait = false;     // 是否等待按键后继续（对应 nextLine）
};

// 分支选项
struct StoryOption {
    char key = 'a';                // 触发键
    std::string text;              // 选项文本
    int branch = 0;                // 分支 id
    std::vector<StoryLine> lines;  // 选中后的叙事
};

// 分支选择
struct StoryChoice {
    std::string prompt;             // 选择提示文本
    std::vector<StoryOption> options;
};

// 一幕场景
struct Scene {
    int id = 0;
    std::vector<StoryLine> lines;  // 顺序播放的叙事
    StoryChoice choice;            // 可选分支（options 为空表示无分支）
};

// 整个剧情
struct StoryData {
    std::vector<Scene> scenes;
};

// 从 JSON 文件加载剧情（解析失败或文件缺失时抛 std::runtime_error）
StoryData loadStory(const std::string& path);
