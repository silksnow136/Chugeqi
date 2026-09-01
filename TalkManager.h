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
    // 第一幕
    bool talkScene01(Game& game1, int branch_id);
    // 第二幕
    bool talkScene02(Game& game1, int branch_id);
    // 第三幕
    bool talkScene03(Game& game1, int branch_id);

    // 对话人物打印

    void talkScene_01();   // 虞姬、小卒a
    void talkScene_020();  // 农夫
    void talkScene_021();  // 王翦、楚军
    void talkScene_022();  // 副将、楚军
    void talkScene_03();   // 钟离昧
    void talkScene_04();   // 钟离昧、乌江亭长、众将士、韩信

    //对话人物选择
    void talkCharacter_01(int character_id, int branch_id);
    void talkCharacter_02(int character_id, int branch_id);
    void talkCharacter_03(int character_id, int branch_id);

    // 当前角色对话

    void talk_character_contnt_01(int current_character);

    void talk_character_contnt_020(int current_character);

    void talk_character_contnt_021(int current_character);

    void talk_character_contnt_022(int current_character);

    void talk_character_contnt_03(int current_character);

    void talk_character_contnt_04(int current_character);

    //打印返回文字
    void waitForReturn();
private:
    SceneManager* sceneManager = nullptr;
};