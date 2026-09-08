// ---------------------------------------------------------------------------
// wujiangMap.cpp — 第四幕「乌江自刎」地图
//
// 27×27 网格。项羽败至乌江，乌江亭长舣船以待。
// 江东近在咫尺，项羽却自言无颜见江东父老。
// 地形：乌江渡口 → 江岸柳林 → 船埠 → 楚军残兵 → 汉军追兵。
// 最终：项羽赠马、诀别、自刎。
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

    MapGrid grid(27, 27);
    grid.setPlayer(20, 13);  // 项羽从南面来至江边

    // ===== 乌江（北面水域）=====
    grid.buildWater(1, 1, 26);   // 江面第一排
    grid.buildWater(2, 1, 26);   // 江面第二排
    grid.buildWater(3, 1, 26);   // 江面第三排

    // ===== 船埠/渡口 =====
    grid.setTile(4, 12, "船埠", TileType::DOOR, "船埠");
    grid.setTile(4, 13, "船埠", TileType::DOOR, "船埠");
    grid.setTile(5, 12, "小舟", TileType::FRIEND, "乌江亭长");
    grid.setTile(5, 13, "乌篷", TileType::FRIEND, "渡船");

    // ===== 江岸柳林 =====
    grid.buildCheval(6, 5);
    grid.buildCheval(6, 20);
    grid.buildCheval(8, 4);
    grid.buildCheval(8, 21);
    grid.buildCheval(10, 3);
    grid.buildCheval(10, 22);
    grid.setTile(7, 8, "老柳", TileType::WALL);
    grid.setTile(7, 18, "老柳", TileType::WALL);
    grid.setTile(9, 10, "残碑", TileType::WALL);
    grid.setTile(9, 16, "残碑", TileType::WALL);

    // ===== 岸边小筑 =====
    grid.buildRoom(6, 11, 9, 16, 13);  // 亭子（门在13列）

    // ===== 楚军残兵（仅剩数骑）=====
    grid.setTile(18, 11, "楚骑", TileType::FRIEND, "楚骑·甲");
    grid.setTile(18, 15, "楚骑", TileType::FRIEND, "楚骑·乙");
    grid.setTile(19, 10, "楚骑", TileType::FRIEND, "楚骑·丙");
    grid.setTile(19, 16, "楚骑", TileType::FRIEND, "楚骑·丁");
    grid.setTile(21, 11, "副将", TileType::FRIEND, "副将");
    grid.setTile(21, 15, "乌骓", TileType::FRIEND, "乌骓");

    // ===== 物品 =====
    grid.setTile(7, 13, "铜镜", TileType::ITEM, "虞姬铜镜");
    grid.setTile(20, 13, "楚旗", TileType::ITEM, "楚军残旗");
    grid.setTile(22, 8, "楚酒", TileType::ITEM, "楚酒（半坛）");

    // ===== 汉军追兵（南面涌来）=====
    grid.setTile(25, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(24, 8, "吕马", TileType::ENEMY, "吕马童");
    grid.setTile(24, 18, "王翳", TileType::ENEMY, "王翳");

    // ===== 功能 =====
    grid.setTile(22, 5, "篝火", TileType::PHARMACY, "残火（疗伤）");
    grid.setTile(22, 21, "石砧", TileType::FORGE, "石砧（磨剑）");

    // ===== 交互回调 =====
    grid.onTalk = [](const std::string& name) {
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        if (name == "乌江亭长") {
            console::setColor(11);
            std::cout << "亭长：“江东虽小，地方千里，众数十万，亦足王也。" << std::endl;
            std::cout << "        大王急渡！今独臣有船，汉军至，无以渡！”" << std::endl;
            console::setColor(7);
        }
        else if (name == "渡船")  std::cout << "一叶小舟在江风中轻轻摇晃，似在催促。" << std::endl;
        else if (name == "副将")  std::cout << "副将跪地：“大王，渡江吧！留得青山在……”" << std::endl;
        else if (name == "乌骓")  std::cout << "乌骓低首轻鸣，依偎着主人，似有泪光。" << std::endl;
        else if (name == "楚骑·甲") std::cout << "楚骑：“大王去，我等断后！”" << std::endl;
        else                      std::cout << "楚骑：“……不走了。”" << std::endl;
        console::pause();
    };

    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        if (name == "虞姬铜镜") {
            std::cout << "铜镜背面映出一张疲惫的面容。" << std::endl;
            std::cout << "项羽：“力拔山兮气盖世，时不利兮骓不逝……”" << std::endl;
        }
        else if (name == "楚军残旗") std::cout << "旗上血迹已干，楚字犹在。" << std::endl;
        else if (name == "楚酒（半坛）") std::cout << "半坛残酒，敬江东父老。" << std::endl;
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
        if (name == "吕马童")  std::cout << "吕马童：“那是项王……”（旧识，犹豫不前）" << std::endl;
        else if (name == "王翳") std::cout << "王翳：“取项羽首级者，封万户侯！”" << std::endl;
        console::pause();
    };

    grid.onPharmacy = [](const std::string& name) {
        console::setColor(11);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "残火尚暖，最后疗伤的机会。" << std::endl;
        console::pause();
    };

    grid.onForge = [](const std::string& name) {
        console::setColor(13);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "石砧上磨剑，锋刃映着乌江波光。" << std::endl;
        std::cout << "项羽：“今日当以此剑，全霸王之名。”" << std::endl;
        console::pause();
    };

    // ===== 主循环 =====
    grid.render();
    if (argc > 1) return 0;

    std::cout << "乌江自刎 —— WASD 移动项羽，B 背包，ESC 退出" << std::endl;
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
