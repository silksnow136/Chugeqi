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

    // 进入对话系统
    void talkManager(int scene_id = 0, int branch_id = 0);

    // 对话系统
    bool talkScene(Game& game1, int branch_id);
    //播放对话
    void talkCharacter(int scene_id, int character_id, int branch_id);
    // 第一幕
    void talkScene01(Game& game1, int branch_id);
    // 第二幕
    void talkScene02(Game& game1, int branch_id);
    // 第三幕
    void talkScene03(Game& game1, int branch_id);
    //第四幕
    void talkScene04(Game& game1, int branch_id);
    // 对话人物打印

    void talkScene_01();   // 虞姬、小卒a
    void talkScene_020();  // 农夫
    void talkScene_021();  // 王翦、楚军
    void talkScene_022();  // 副将、楚军
    void talkScene_03();   // 钟离昧
    void talkScene_04();   // 钟离昧、乌江亭长、众将士、韩信

    
    // 从 JSON 读取对话
    void TalkManager::loadDialogue(
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