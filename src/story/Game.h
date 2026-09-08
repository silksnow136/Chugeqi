#pragma once

#include <string>
#include <memory>
#include "SceneManager.h"
#include "core/dataLoader.h"

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

private:
    bool running = true;

    void initialize();
    void gameLoop();

    void showWelcome();
    void showHelp();

    int scene_id = 0;

    // 全局游戏数据
    GameData gameData;

    // 当前玩家
    std::unique_ptr<Combatant> player;

    // 当前金币
    int gold = 100;

    SceneManager sceneManager;
};