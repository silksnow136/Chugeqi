#pragma once

#include <string>
using namespace std;

class Game;
class SceneManager;

// 对话系统管理
class TalkManager
{
public:
    TalkManager();

    void setSceneManager(SceneManager* manager);

    // 外部直接播放指定人物对话（不刷新场景地图，供地图移动交互调用）
    void talkCharacterExternal(int scene_id, int character_id, int branch_id);

    // 从 JSON 读取对话
    void loadDialogue(
        int scene_id,
        int branch_id,
        int character_id
    );

    // 播放一句对话
    void playDialogue(
        std::string speaker,
        std::string text
    );

    //打印返回文字
    void waitForReturn();
private:
    SceneManager* sceneManager = nullptr;
};
