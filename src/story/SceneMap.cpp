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

// 按 NPC 名称匹配对应战斗配置（普通汉军兜底）
static const char* battleFileForEnemy(const std::string& npc) {
    if (npc == "汉军哨骑") return "data/battles/battle_sentinel.json";
    if (npc == "汉军铁骑") return "data/battles/battle_iron_cavalry.json";
    if (npc == "汉军斥候") return "data/battles/battle_scout.json";
    if (npc == "灌婴")     return "data/battles/battle_guanying.json";
    if (npc == "王翳")     return "data/battles/battle_wangyi.json";
    return "data/battles/battle_han_soldier.json";
}

// 地图遭遇战：按 NPC 名加载对应战斗配置，返回是否胜利
static bool runBattle(Game& game, Combatant& player, const std::string& enemyName) {
    console::clearScreen();
    console::setColor(12);
    std::cout << "===== 遭遇 " << enemyName << "！战斗开始！ =====" << std::endl;
    console::setColor(7);
    console::pause();

    bool won = false;
    try {
        Battle battle = DataLoader::loadBattle(battleFileForEnemy(enemyName), game.getGameData());

        Combatant* p = &player;
        std::vector<Combatant*> companions;
        if (Combatant* c = game.getCompanion(); c != nullptr && c->isAlive()) {
            companions.push_back(c);
        }
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

// 主线地图名（按幕次）
static std::string mainMapName(int scene_id) {
    switch (scene_id) {
        case 1: return "垓下营地";
        case 2: return "淮河";
        default: return "";
    }
}

// 按地图名构建网格：主线地图按 scene_id 布局，子地图按名称
static MapGrid buildMapByName(const std::string& name, int scene_id, int branch_id, const Combatant& player) {
    if (name == "垓下营地" || name == "淮河")
        return MapLayouts::buildSceneMap(scene_id, branch_id);
    return MapLayouts::buildNamedMap(name, player);
}

// 应用已清除的格子（击败的敌人 / 拾取的道具），保证往返地图不刷新
static void applyCleared(MapGrid& grid, const WorldState& world, const std::string& mapName) {
    auto it = world.cleared.find(mapName);
    if (it == world.cleared.end()) return;
    for (int code : it->second) grid.clearTile(code / 1000, code % 1000);
}

bool runSceneMap(Game& game, SceneManager& sm, int scene_id, int branch_id) {
    QuestState& qs = sm.getQuestState();
    Combatant& player = game.getPlayer();
    WorldState& world = game.getWorld();

    // 当前地图：读档恢复用保存的地图名，否则按幕次取主线地图
    if (world.mapName.empty()) world.mapName = mainMapName(scene_id);

    MapGrid grid = buildMapByName(world.mapName, scene_id, branch_id, player);
    applyCleared(grid, world, world.mapName);
    if (world.playerRow >= 0 && world.playerCol >= 0) {
        grid.setPlayer(world.playerRow, world.playerCol); // 读档恢复坐标
    }

    // 单一交互回调：[&] 捕获，world.mapName 随传送更新，无需按地图重新实现
    auto interact = [&](TileType type, const std::string& name, int row, int col) {
        switch (type) {
            case TileType::FRIEND: {
                if (SideQuest::tryHandleTalk(game, player, qs, world.mapName, name)) break;
                if (sm.getTalkManager().talkCharacterExternal(scene_id, name, branch_id)) break;
                sm.getTalkManager().playSimpleTalk(name);
                break;
            }
            case TileType::ENEMY: {
                if (runBattle(game, player, name)) {
                    SideQuest::onBattleWon(qs, world.mapName, name);
                    grid.clearTile(row, col);
                    world.markCleared(world.mapName, row, col); // 击败后不再刷新
                } else if (!player.isAlive()) {
                    game.showDefeatEnding(); // 玩家战死 → 败亡结算
                }
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
                world.markCleared(world.mapName, row, col); // 拾取后不再刷新
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
                if (scene_id < 3)
                    std::cout << "\n是否进入第 " << (scene_id + 1) << " 幕？（Y 确认 / N 留在当前场景）" << std::endl;
                else
                    std::cout << "\n是否突围，奔赴乌江？（Y 确认 / N 留在战场）" << std::endl;
                console::setColor(7);
                int key = console::readKey();
                if (key == 'y' || key == 'Y') {
                    grid.advanceTriggered = true;
                    world.mapName.clear();          // 进入新幕，重置地图定位
                    world.playerRow = world.playerCol = -1;
                    if (scene_id < 3) {
                        sm.changeScene(scene_id + 1);
                        sm.ShowBackground(scene_id + 1);
                    } else {
                        sm.playEnding();            // 终幕突围 → 结局
                    }
                }
                break;
            }
            case TileType::PORTAL: {
                if (SideQuest::canEnterPortal(player, qs, world.mapName, name)) {
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
    std::cout << "第" << scene_id << "幕 · " << world.mapName
              << " —— WASD 移动，白色箭头=传送门，E 存读档，ESC 退出场景" << std::endl;

    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'b') { // 背包与属性
            showBackpack(&player, game.getItemPool());
            grid.render();
            continue;
        }
        if (dir == 'e') { // 存读档
            if (game.saveMenu()) {
                return true; // 发生了读档，已切换到新地图，结束当前循环
            }
            grid.render();
            continue;
        }
        if (dir != 'w' && dir != 'a' && dir != 's' && dir != 'd') continue;

        grid.move(dir);

        if (!game.isRunning()) break; // 结局结算后退出

        if (grid.portalTriggered) {
            std::string target = grid.portalTarget;
            grid.portalTriggered = false;
            grid.portalTarget.clear();
            grid.advanceTriggered = false;
            world.mapName = target;
            SideQuest::onEnterMap(qs, world.mapName);
            grid = MapLayouts::buildNamedMap(target, player);
            grid.onInteract = interact;  // grid 重建后重新绑定回调
            applyCleared(grid, world, world.mapName);
            world.playerRow = grid.getPlayerRow();
            world.playerCol = grid.getPlayerCol();
            grid.render();
            std::cout << "当前位置：" << world.mapName << std::endl;
            continue;
        }

        if (grid.advanceTriggered) break;

        world.playerRow = grid.getPlayerRow();
        world.playerCol = grid.getPlayerCol();
        grid.render();
    }

    console::clearScreen();
    world.mapName.clear();          // 退出地图，不再处于地图内
    world.playerRow = world.playerCol = -1;
    return true;
}

} // namespace SceneMap
