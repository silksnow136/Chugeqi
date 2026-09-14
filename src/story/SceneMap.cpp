#include "SceneMap.h"
#include "QuestState.h"
#include "Game.h"
#include "SceneManager.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "backpack.h"
#include "MapLayouts.h"
#include "SideQuest.h"
#include "core/console.h"
#include "data/dataLoader.h"
#include "combat/combatSystem.h"
#include <iostream>
#include <memory>
#include <cctype>

namespace SceneMap {

// 地图遭遇战：复用 battle_test.json，返回是否胜利
static bool runBattle(Game& game, Combatant& player, const std::string& enemyName) {
    console::clearScreen();
    console::setColor(12);
    std::cout << "===== 遭遇 " << enemyName << "！战斗开始！ =====" << std::endl;
    console::setColor(7);
    console::pause();

    bool won = false;
    try {
        Battle battle = DataLoader::loadBattle("data/battle_test.json", game.getGameData());

        Combatant* p = &player;
        std::vector<Combatant*> companions;  // 地图遭遇战不带同伴
        std::vector<Combatant*> enemies;
        for (const auto& e : battle.enemies) enemies.push_back(e.get());

        CombatSystem combat(p, companions, enemies, battle.config, &game.getGameData().itemPool);
        won = combat.startBattle();

        console::clearScreen();
        console::setColor(won ? 10 : 12);
        std::cout << (won ? "战斗胜利！击败了 " : "战斗失败...") << (won ? enemyName : "") << std::endl;
        console::setColor(7);
        std::cout << "按任意键返回地图" << std::endl;
        console::pause();
    } catch (const std::exception& e) {
        std::cerr << "战斗系统错误: " << e.what() << std::endl;
    }
    return won;
}

bool runSceneMap(Game& game, SceneManager& sm, int scene_id, int branch_id) {
    QuestState& qs = sm.getQuestState();
    Combatant& player = game.getPlayer();

    std::string mapName;
    switch (scene_id) {
        case 1:  mapName = "垓下营地"; break;
        case 2:  mapName = "淮河"; break;
        case 3:  mapName = "东城"; break;
        default: mapName = "乌江"; break;
    }

    MapGrid grid = MapLayouts::buildSceneMap(scene_id, branch_id);

    // 单一交互回调：[&] 捕获，mapName 随传送更新，无需按地图重新实现
    auto interact = [&](TileType type, const std::string& name) {
        switch (type) {
            case TileType::FRIEND: {
                if (SideQuest::tryHandleTalk(game, player, qs, mapName, name)) break;
                if (sm.getTalkManager().talkCharacterExternal(scene_id, name, branch_id)) break;
                sm.getTalkManager().playSimpleTalk(name);
                break;
            }
            case TileType::ENEMY: {
                if (runBattle(game, player, name)) SideQuest::onBattleWon(qs, mapName, name);
                break;
            }
            case TileType::PHARMACY: {
                console::clearScreen();
                console::setColor(11);
                std::cout << "===== " << name << " =====" << std::endl;
                console::setColor(7);
                sm.enterPharmacy(game);
                break;
            }
            case TileType::ITEM: {
                player.addItem(name, 1);  // name 即物品 ID
                const ItemPool& pool = game.getItemPool();
                auto it = pool.find(name);
                std::string displayName = (it != pool.end()) ? it->second->getName() : name;
                console::setColor(14);
                std::cout << "\n[拾取] 获得「" << displayName << "」！" << std::endl;
                console::setColor(7);
                console::pause();
                break;
            }
            case TileType::ADVANCE: {
                console::clearScreen();
                console::setColor(13);
                std::cout << "\n===== " << name << " =====" << std::endl;
                console::setColor(7);
                sm.printWords(sm.getAdvancePrompt(scene_id), 11, 500, 80);
                if (scene_id == 1) {
                    std::string insert = SideQuest::advanceNarration(qs, scene_id);
                    if (!insert.empty()) sm.printWords(insert, 14, 300, 60);
                }
                console::setColor(14);
                if (scene_id < 4)
                    std::cout << "\n是否进入第 " << (scene_id + 1) << " 幕？（Y 确认 / N 留在当前场景）" << std::endl;
                else
                    std::cout << "\n是否渡江？（Y 确认 / N 留在江畔）" << std::endl;
                console::setColor(7);
                int key = console::readKey();
                if (key == 'y' || key == 'Y') {
                    grid.advanceTriggered = true;
                    if (scene_id < 4) { sm.changeScene(scene_id + 1); sm.ShowBackground(scene_id + 1); }
                    else { sm.ShowBackground(4); }
                }
                break;
            }
            case TileType::PORTAL: {
                if (SideQuest::canEnterPortal(player, qs, mapName, name)) {
                    grid.portalTriggered = true;
                    grid.portalTarget = name;
                }
                break;
            }
            default: break;
        }
    };

    grid.onInteract = interact;

    // ===== WASD 主循环 =====
    grid.render();
    std::cout << "第" << scene_id << "幕 · " << mapName
              << " —— WASD 移动，白色箭头=传送门，ESC 退出场景" << std::endl;

    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'b') { // 背包与属性
            showBackpack(&player, game.getItemPool());
            grid.render();
            continue;
        }
        if (dir != 'w' && dir != 'a' && dir != 's' && dir != 'd') continue;

        grid.move(dir);

        if (grid.portalTriggered) {
            std::string target = grid.portalTarget;
            grid.portalTriggered = false;
            grid.portalTarget.clear();
            grid.advanceTriggered = false;
            mapName = target;
            SideQuest::onEnterMap(qs, mapName);
            grid = MapLayouts::buildNamedMap(target, player);
            grid.onInteract = interact;  // grid 重建后重新绑定回调
            grid.render();
            std::cout << "当前位置：" << mapName << std::endl;
            continue;
        }

        if (grid.advanceTriggered) break;

        grid.render();
    }

    console::clearScreen();
    return true;
}

} // namespace SceneMap
