#pragma once

#include <string>
#include <memory>
#include "SceneManager.h"
#include "data/dataLoader.h"

// 负责控制游戏初始化以及指令处理
class Game {
public:
    Game();

    void run();
    void gameCommand(const std::string& command);

    // 玩家
    Combatant& getPlayer();

    // 药店需要
    ItemPool& getItemPool();
    int& getGold();


    GameData& getGameData();

    // 存档 / 读档（slot 1~2，供命令层与地图场景调用）
    void doSave(int slot);
    void doLoad(int slot);

    // 存读档界面（清屏显示槽位与信息，单键操作；返回 true 表示发生了读档）
    bool saveMenu();


private:
    bool running = true;

    void initialize();
    void gameLoop();

    void showWelcome();
    void showHelp();

    bool slotMenu(int slot);

    int scene_id = 0;

    // 全局游戏数据
    GameData gameData;

    // 当前玩家
    std::unique_ptr<Combatant> player;

    // 当前金币
    int gold = 100;

    SceneManager sceneManager;
};