#include "combatSystem.h"
#include "data/dataLoader.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

// 战斗模拟器：无交互跑一场指定战斗（双方 AI 托管），用于量化难度与平衡性。
// 用法：battleSim <battleId> [playerLevel]
//   例：battleSim wangyi 2   —— 玩家升到 2 级后打「遭遇王翳」

static void levelUpTo(Combatant* c, int targetLevel) {
    while (c->getLevel() < targetLevel) {
        c->addExp(10 * c->getLevel() * c->getLevel());
    }
}

int main(int argc, char** argv) {
    std::string battleId = argc > 1 ? argv[1] : "wangyi";
    int level = argc > 2 ? std::atoi(argv[2]) : 0;

    try {
        GameData gd = DataLoader::loadGameData("data/core/");
        auto player = DataLoader::loadCombatant("data/characters/player.json", gd.skillPool);
        auto companion = DataLoader::loadCombatant("data/characters/companion.json", gd.skillPool);

        if (level > 0) {
            levelUpTo(player.get(), level);
            levelUpTo(companion.get(), level);
        }

        Battle battle = DataLoader::loadBattle("data/battles/battle_" + battleId + ".json", gd);

        std::vector<Combatant*> enemies;
        for (auto& e : battle.enemies) enemies.push_back(e.get());
        std::vector<Combatant*> companions;
        if (companion->isAlive()) companions.push_back(companion.get());

        CombatSystem combat(player.get(), companions, enemies, battle.config, &gd.itemPool, true);
        bool win = combat.startBattle();

        std::cout << "战斗[" << battleId << "] 玩家Lv." << player->getLevel()
                  << " => " << (win ? "胜利" : "失败") << "\n";
        std::cout << "项羽   HP " << player->getHP() << "/" << player->getMaxHP()
                  << " SP " << player->getSP() << "/" << player->getMaxSP() << "\n";
        if (companion->isAlive())
            std::cout << "钟离昧 HP " << companion->getHP() << "/" << companion->getMaxHP() << "\n";
        else
            std::cout << "钟离昧 阵亡\n";
        std::cout << "--- 战斗日志 ---\n";
        for (const auto& l : combat.getLog()) std::cout << "  " << l << "\n";
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
