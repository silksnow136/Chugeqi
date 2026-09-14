#pragma once

#include <string>

class Game;
class SceneManager;

// 对话系统管理
class TalkManager
{
public:
    TalkManager();

    void setSceneManager(SceneManager* manager);

    // 按 NPC 名播放 talk.json 中的人物对话；返回是否找到（未找到则走简单对话）
    bool talkCharacterExternal(int scene_id, const std::string& name, int branch_id);

    // 简单对话（talk.json 的 "simple" 表，供未接入主对话的 NPC）
    void playSimpleTalk(const std::string& name);

    // 播放一句对话
    void playDialogue(std::string speaker, std::string text);

    //打印提示并等待按键返回
    void waitForReturn();
private:
    // 从 talk.json 按人物名读取并播放对话，返回是否找到
    bool loadDialogue(int scene_id, int branch_id, const std::string& name);

    SceneManager* sceneManager = nullptr;
};
