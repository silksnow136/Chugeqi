// ---------------------------------------------------------------------------
// huaiRiverMap.cpp — 第二幕「淮河突围」地图
//
// 37×27 网格。项羽率八百骑夜出垓下，南渡淮河。
// 地形：南岸营地→河滩→淮河（急流）→北岸丛林→汉军追击线。
// 河面用 ~（不可通行），浅滩用门（可渡河）。汉军从南面追来。
// ---------------------------------------------------------------------------

#include "story/MapGrid.h"
#include "core/console.h"
#include "combat/character.h"
#include "combat/item.h"
#include "core/dataLoader.h"
#include <iostream>
#include <cctype>

int main(int argc, char* argv[]) {
    console::init();

    // 加载物品/技能数据与玩家角色（含初始背包）
    GameData gameData = DataLoader::loadGameData("data/");
    auto player = DataLoader::loadCombatant("data/player.json", gameData.skillPool);

    MapGrid grid(37, 27);
    grid.setPlayer(30, 13);  // 项羽从南岸出发

    // ===== 淮河（横向河流，中间有浅滩可渡）=====
    grid.buildWater(17, 1, 6);   // 左段急流
    grid.buildWater(17, 20, 6);  // 右段急流
    grid.buildWater(18, 1, 5);   // 第二排
    grid.buildWater(18, 21, 5);
    grid.buildWater(19, 1, 4);
    grid.buildWater(19, 22, 4);
    // 浅滩（门）：可渡河的位置
    grid.setTile(17, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(17, 13, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(18, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(18, 13, "浅滩", TileType::DOOR, "浅滩");

    // ===== 北岸（丛林/高地）=====
    grid.buildCheval(5, 8);
    grid.buildCheval(5, 18);
    grid.buildCheval(8, 6);
    grid.buildCheval(8, 20);
    grid.buildCheval(12, 10);
    grid.buildCheval(12, 16);

    // ===== 北岸建筑 =====
    grid.buildRoom(3, 3, 8, 8, 5);      // 丛林哨所
    grid.buildRoom(3, 19, 8, 24, 22);   // 废弃农舍

    // ===== 南岸营地（残破）=====
    grid.buildRoom(28, 3, 33, 8, 5);     // 残帐
    grid.buildRoom(28, 19, 33, 24, 22);  // 马厩

    // ===== 河滩装饰 =====
    grid.buildWater(16, 1, 26);  // 南岸水线
    grid.buildWater(20, 1, 26);  // 北岸水线

    // ===== 友方 NPC =====
    grid.setTile(6, 5, "哨兵", TileType::FRIEND, "哨兵");
    grid.setTile(6, 22, "农人", TileType::FRIEND, "农人");
    grid.setTile(30, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(31, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(30, 22, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(31, 22, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(28, 13, "向导", TileType::FRIEND, "向导");
    grid.setTile(32, 13, "斥候", TileType::FRIEND, "斥候");
    grid.setTile(14, 13, "渔夫", TileType::FRIEND, "渔夫");

    // ===== 物品 =====
    grid.setTile(6, 6, "渡图", TileType::ITEM, "渡河图");
    grid.setTile(30, 6, "楚旗", TileType::ITEM, "残破楚旗");
    grid.setTile(14, 14, "蓑衣", TileType::ITEM, "蓑衣");

    // ===== 汉军追兵（南面）=====
    grid.setTile(35, 5, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 6, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 13, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 14, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 20, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 21, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(34, 8, "灌婴", TileType::ENEMY, "灌婴");

    // ===== 功能建筑 =====
    grid.setTile(6, 22, "农舍", TileType::PHARMACY, "农舍（草药）");
    grid.setTile(30, 5, "残帐", TileType::FORGE, "残帐（修补）");

    // ===== 交互回调 =====
    grid.onTalk = [](const std::string& name) {
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        if (name == "渔夫")       std::cout << "渔夫：“水浅处可渡，但须趁雾，天明便来不及了。”" << std::endl;
        else if (name == "向导")  std::cout << "向导：“大王，过河北去，东城尚有路径。”" << std::endl;
        else if (name == "斥候")  std::cout << "斥候：“汉军灌婴已追至，约三千骑！”" << std::endl;
        else if (name == "楚骑兵") std::cout << "楚骑：“愿随大王死战突围！”" << std::endl;
        else if (name == "马夫")  std::cout << "马夫：“乌骓认得浅滩，可循之渡河。”" << std::endl;
        else if (name == "乌骓")  std::cout << "乌骓踏水而起，鬃毛沾满河雾。" << std::endl;
        else if (name == "哨兵")  std::cout << "哨兵：“北岸似乎安全……但林中有异响。”" << std::endl;
        else if (name == "农人")  std::cout << "农人：“军爷，拿些草药走吧。”" << std::endl;
        else                      std::cout << "（此处无可对话内容）" << std::endl;
        console::pause();
    };

    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        if (name == "渡河图")      std::cout << "渡河路径图，标注了浅滩位置。" << std::endl;
        else if (name == "残破楚旗") std::cout << "沾血的楚军大旗，士气之所系。" << std::endl;
        else if (name == "蓑衣")   std::cout << "渔夫的蓑衣，渡河时可避寒水。" << std::endl;
        // 拾取入包：按名称匹配物品池定义，加入角色背包
        const Item* it = findItemByName(gameData.itemPool, name);
        if (it != nullptr) {
            player->addItem(it->getId(), 1);
            std::cout << "「" << name << "」已放入背包。（按 B 打开背包查看）" << std::endl;
        } else {
            std::cout << "（物品池中未找到「" << name << "」的定义）" << std::endl;
        }
        console::pause();
    };

    grid.onBattle = [](const std::string& name) {
        console::setColor(12);
        std::cout << "\n===== 遭遇 " << name << "！战斗（占位） =====" << std::endl;
        console::setColor(7);
        console::pause();
    };

    grid.onPharmacy = [](const std::string& name) {
        console::setColor(11);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "农人：“采了些止血草，将军拿去。”" << std::endl;
        console::pause();
    };

    grid.onForge = [](const std::string& name) {
        console::setColor(13);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "军士在残帐中修补甲胄。" << std::endl;
        console::pause();
    };

    // ===== 主循环 =====
    grid.render();
    if (argc > 1) return 0;

    std::cout << "淮河突围 —— WASD 移动项羽，B 背包，ESC 退出" << std::endl;
    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'w' || dir == 'a' || dir == 's' || dir == 'd') {
            grid.move(dir);
            grid.render();
        } else if (dir == 'b') {
            showBackpack(player.get(), gameData.itemPool);
            grid.render();
        }
    }

    console::clearScreen();
    console::setColor(14);
    std::cout << "已退出。" << std::endl;
    console::setColor(7);
    return 0;
}
