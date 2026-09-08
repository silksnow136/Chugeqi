// ---------------------------------------------------------------------------
// dongchengMap.cpp — 第三幕「东城快战」地图
//
// 27×27 网格。项羽渡淮后仅余二十八骑，至东城。
// 汉军数千追至，四面合围。项羽据丘陵，二十八骑列阵迎击。
// 地形：中央高地 → 四面坡地 → 汉军包围圈。
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
    grid.setPlayer(13, 13);  // 项羽在高地中央

    // ===== 高地（中央丘陵，用拒马围栏标示边界）=====
    grid.buildCheval(9, 9);
    grid.buildCheval(9, 17);
    grid.buildCheval(17, 9);
    grid.buildCheval(17, 17);
    grid.buildCheval(10, 8);
    grid.buildCheval(10, 18);
    grid.buildCheval(16, 8);
    grid.buildCheval(16, 18);

    // 高地斜坡装饰（碎石）
    grid.setTile(8, 13, "碎石", TileType::WALL);
    grid.setTile(18, 13, "碎石", TileType::WALL);
    grid.setTile(13, 8, "碎石", TileType::WALL);
    grid.setTile(13, 18, "碎石", TileType::WALL);

    // ===== 汉军包围圈（四面）=====
    // 北面
    grid.setTile(1, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 12, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(2, 8, "杨喜", TileType::ENEMY, "杨喜");
    grid.setTile(2, 18, "王翳", TileType::ENEMY, "王翳");

    // 南面
    grid.setTile(25, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 12, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(24, 8, "吕胜", TileType::ENEMY, "吕胜");
    grid.setTile(24, 18, "赤侯", TileType::ENEMY, "赤侯");

    // 东面
    grid.setTile(5, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(12, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(13, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(21, 25, "汉骑", TileType::ENEMY, "汉军");

    // 西面
    grid.setTile(5, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(12, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(13, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(21, 1, "汉骑", TileType::ENEMY, "汉军");

    // 灌婴坐镇
    grid.setTile(2, 13, "灌婴", TileType::ENEMY, "灌婴");

    // ===== 二十八骑（友方，高地周围）=====
    grid.setTile(11, 11, "楚骑", TileType::FRIEND, "楚骑·甲");
    grid.setTile(11, 15, "楚骑", TileType::FRIEND, "楚骑·乙");
    grid.setTile(12, 10, "楚骑", TileType::FRIEND, "楚骑·丙");
    grid.setTile(12, 16, "楚骑", TileType::FRIEND, "楚骑·丁");
    grid.setTile(14, 10, "楚骑", TileType::FRIEND, "楚骑·戊");
    grid.setTile(14, 16, "楚骑", TileType::FRIEND, "楚骑·己");
    grid.setTile(15, 11, "楚骑", TileType::FRIEND, "楚骑·庚");
    grid.setTile(15, 15, "楚骑", TileType::FRIEND, "楚骑·辛");
    grid.setTile(11, 13, "副将", TileType::FRIEND, "副将");
    grid.setTile(15, 13, "乌骓", TileType::FRIEND, "乌骓");

    // ===== 物品 =====
    grid.setTile(13, 11, "楚旗", TileType::ITEM, "楚军大旗");
    grid.setTile(13, 15, "太阿", TileType::ITEM, "太阿剑");
    grid.setTile(10, 13, "楚酒", TileType::ITEM, "楚酒（残）");

    // ===== 功能 =====
    grid.setTile(3, 3, "野帐", TileType::PHARMACY, "野战医帐");
    grid.setTile(3, 23, "残炉", TileType::FORGE, "野战铁炉");

    // ===== 交互回调 =====
    grid.onTalk = [](const std::string& name) {
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        if (name == "副将")       std::cout << "副将：“大王，我等二十八骑，誓死一战！”" << std::endl;
        else if (name == "楚骑·甲") std::cout << "楚骑：“愿为大王开路！”" << std::endl;
        else if (name == "乌骓")  std::cout << "乌骓嘶鸣，前蹄刨地，似知此为死战。" << std::endl;
        else                      std::cout << "楚骑：“赴死无悔！”" << std::endl;
        console::pause();
    };

    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        if (name == "楚军大旗")  std::cout << "大旗展开，二十八骑齐声高呼！" << std::endl;
        else if (name == "太阿剑") std::cout << "太阿出鞘，寒光照夜。" << std::endl;
        else if (name == "楚酒（残）") std::cout << "残酒洒地，祭阵亡弟兄。" << std::endl;
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
        if (name == "灌婴")  std::cout << "灌婴：“项羽已穷途末路，休走！”" << std::endl;
        else if (name == "杨喜") std::cout << "杨喜封住北逃之路。" << std::endl;
        console::pause();
    };

    grid.onPharmacy = [](const std::string& name) {
        console::setColor(11);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "军医：“最后一些金创药了，省着用。”" << std::endl;
        console::pause();
    };

    grid.onForge = [](const std::string& name) {
        console::setColor(13);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "铁匠在野炉旁匆忙修补兵器。" << std::endl;
        console::pause();
    };

    // ===== 主循环 =====
    grid.render();
    if (argc > 1) return 0;

    std::cout << "东城快战 —— WASD 移动项羽，B 背包，ESC 退出" << std::endl;
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
