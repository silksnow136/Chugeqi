// ---------------------------------------------------------------------------
// mapTestMain.cpp — 地图移动系统测试
//
// 测试内容：
//   1. WASD 控制项羽在网格地图中移动
//   2. 边界墙壁不可穿越
//   3. 碰到友方单位 → 触发对话
//   4. 碰到敌方单位 → 触发战斗（接入真实 CombatSystem）
//   5. 碰到药店 → 触发购买（占位）
//   6. 碰到铁匠铺 → 触发锻造（占位）
//   7. 按 ESC 退出测试
// ---------------------------------------------------------------------------

#include "story/MapGrid.h"
#include "core/console.h"
#include "core/dataLoader.h"
#include "combat/combatSystem.h"
#include "combat/saveManager.h"
#include <iostream>
#include <exception>

// 触发真实战斗：加载战斗数据并启动 CombatSystem
static void triggerBattle(const std::string& enemyName) {
    try {
        console::clearScreen();
        console::setColor(12);
        std::cout << "===== 遭遇敌人：" << enemyName << "！战斗开始！ =====" << std::endl;
        console::setColor(7);
        console::pause();

        // 加载战斗数据
        GameData gameData = DataLoader::loadGameData("data/");
        SaveManager save("save.db");

        // 我方队伍
        std::vector<std::unique_ptr<Combatant>> party = save.loadParty(gameData.skillPool);
        if (party.empty()) {
            party = DataLoader::loadPartyTemplates("data/battle_test.json", gameData.skillPool);
            save.saveParty(party, gameData.skillPool);
        }

        // 敌方
        Battle battle = DataLoader::loadBattle("data/battle_test.json", gameData);

        Combatant* player = party[0].get();
        std::vector<Combatant*> companions;
        for (size_t i = 1; i < party.size(); i++) companions.push_back(party[i].get());
        std::vector<Combatant*> enemies;
        for (const auto& e : battle.enemies) enemies.push_back(e.get());

        CombatSystem combat(player, companions, enemies, battle.config);
        bool won = combat.startBattle();

        // 战斗结果写回存档
        save.saveParty(party, gameData.skillPool);

        console::clearScreen();
        if (won) {
            console::setColor(10);
            std::cout << "战斗胜利！击败了 " << enemyName << std::endl;
        } else {
            console::setColor(12);
            std::cout << "战斗失败...项羽阵亡。" << std::endl;
        }
        console::setColor(7);
        std::cout << "按任意键返回地图" << std::endl;
        console::pause();
    } catch (const std::exception& e) {
        std::cerr << "战斗系统错误: " << e.what() << std::endl;
        console::pause();
    }
}

int main() {
    try {
        console::init();

        // 创建 13x16 的网格地图（每格 4 列宽 → 总宽 64 字符，适合终端）
        MapGrid grid(13, 16);

        // 设置玩家（项羽）初始位置
        grid.setPlayer(6, 2);

        // 放置友方单位（全称显示）
        grid.setTile(2, 4, "虞姬", TileType::FRIEND, "虞姬");
        grid.setTile(4, 8, "小卒", TileType::FRIEND, "小卒a");
        grid.setTile(9, 12, "副将", TileType::FRIEND, "副将");

        // 放置敌方单位
        grid.setTile(2, 12, "汉军", TileType::ENEMY, "汉军小队");
        grid.setTile(8, 4, "王翦", TileType::ENEMY, "王翦");

        // 放置药店
        grid.setTile(10, 8, "药店", TileType::PHARMACY, "军营药店");

        // 放置铁匠铺
        grid.setTile(2, 8, "铁匠", TileType::FORGE, "军营铁匠");

        // 设置交互回调
        grid.onTalk = [](const std::string& name) {
            console::setColor(10);
            std::cout << "\n===== 与 " << name << " 对话 =====" << std::endl;
            console::setColor(7);
            // 占位：实际接入 TalkManager 后这里会调用对话系统
            std::cout << name << "：将军，如今局势危急，需早做决断。" << std::endl;
            std::cout << "（对话系统占位 —— 后续接入 TalkManager）" << std::endl;
            console::pause();
        };

        grid.onBattle = [](const std::string& name) {
            triggerBattle(name);
        };

        grid.onPharmacy = [](const std::string& name) {
            console::setColor(11);
            std::cout << "\n===== 进入 " << name << " =====" << std::endl;
            console::setColor(7);
            std::cout << "药铺老板：将军要买些金创药吗？" << std::endl;
            std::cout << "（药店系统占位 —— 后续接入 PharManager）" << std::endl;
            console::pause();
        };

        grid.onForge = [](const std::string& name) {
            console::setColor(13);
            std::cout << "\n===== 进入 " << name << " =====" << std::endl;
            console::setColor(7);
            std::cout << "铁匠：将军的兵器可要打磨一番？" << std::endl;
            std::cout << "（锻造系统占位 —— 后续接入 ForgeManager）" << std::endl;
            console::pause();
        };

        // 主循环
        grid.render();
        std::cout << "测试已启动！WASD 移动项羽，ESC 退出。" << std::endl;

        while (true) {
            int key = console::readKey();

            // ESC 退出
            if (key == 27) {
                console::clearScreen();
                console::setColor(14);
                std::cout << "已退出地图测试。" << std::endl;
                console::setColor(7);
                break;
            }

            // WASD 移动
            char dir = static_cast<char>(std::tolower(key));
            if (dir == 'w' || dir == 'a' || dir == 's' || dir == 'd') {
                bool triggered = grid.move(dir);
                grid.render();

                if (triggered) {
                    // 交互已在回调中处理，这里只需重新渲染
                    grid.render();
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
