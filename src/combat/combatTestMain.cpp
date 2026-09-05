#include "combatSystem.h"
#include "core/dataLoader.h"
#include "saveManager.h"
#include "core/console.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // 1. 静态模板（JSON，长期维护）
        GameData gameData = DataLoader::loadGameData("data/");

        // 2. 存档（SQLite，玩家运行时状态）
        SaveManager save("save.db");

        // 3. 我方队伍：优先从存档加载；首次运行从 JSON 模板建档并落库
        std::vector<std::unique_ptr<Combatant>> party = save.loadParty(gameData.skillPool);
        if (party.empty()) {
            party = DataLoader::loadPartyTemplates("data/battle_test.json", gameData.skillPool);
            save.saveParty(party, gameData.skillPool);
        }

        // 4. 敌方与配置（JSON 模板，每场战斗单独加载）
        Battle battle = DataLoader::loadBattle("data/battle_test.json", gameData);

        Combatant* player = party[0].get();
        std::vector<Combatant*> companions;
        for (size_t i = 1; i < party.size(); i++) companions.push_back(party[i].get());
        std::vector<Combatant*> enemies;
        for (const auto& e : battle.enemies) enemies.push_back(e.get());

        CombatSystem combat(player, companions, enemies, battle.config);
        combat.startBattle();

        // 5. 战斗结束，把队伍最新状态写回存档
        save.saveParty(party, gameData.skillPool);

        // 6. 询问是否重置存档（避免反复测试导致等级不断上升）
        console::init();
        std::cout << "是否重置存档？（下次运行将从初始模板建档）" << std::endl;
        std::cout << "[Y]重置  [其他键]保留" << std::endl;
        int key = console::readKey();
        if (key == 'y' || key == 'Y') {
            save.resetSave();
            std::cout << std::endl << "存档已重置。" << std::endl;
        } else {
            std::cout << std::endl << "已保留存档。" << std::endl;
        }
        console::pause();

        // 资源由 unique_ptr 自动释放
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
