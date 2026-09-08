// ---------------------------------------------------------------------------
// gaiXiaMap.cpp — 第一幕「垓下营地」地图效果展示
//
// 37×27 网格。装饰墙拟物：城墙 █、帐墙 =、栅栏 ║、拒马 ╳、护城河 ~。
// 房间内部留足通道（门内无遮挡），布置 NPC / 物品 / 功能建筑 / 汉军。
// WASD 移动项羽，ESC 退出；带任意参数只渲染一次。
// ---------------------------------------------------------------------------

#include "story/MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>

int main(int argc, char* argv[]) {
    console::init();

    MapGrid grid(37, 27);
    grid.setPlayer(13, 14);

    // ===== 房间（帐墙）=====
    grid.buildRoom( 4, 3, 9, 10, 6);     // 粮仓
    grid.buildRoom( 4, 17, 9, 24, 20);   // 马厩
    grid.buildRoom( 13, 3, 16, 8, 5);    // 军械库
    grid.buildRoom( 10, 11, 17, 18, 14); // 帅帐（中央大帐）
    grid.buildRoom( 10, 20, 14, 25, 22); // 虞姬帐
    grid.buildRoom( 19, 3, 23, 10, 6);   // 伤兵营
    grid.buildRoom( 19, 17, 22, 23, 19); // 伙房
    grid.buildRoom( 25, 3, 28, 8, 5);    // 医帐
    grid.buildRoom( 25, 17, 28, 23, 19); // 铁匠铺

    // ===== 南北壁垒（栅栏）=====
    grid.buildFence( 2, 3, 24, 13, 14);
    grid.buildFence( 30, 3, 24, 13, 14);

    // ===== 装饰（拟物障碍）=====
    grid.buildCheval( 8, 12);   // 拒马
    grid.buildCheval( 8, 15);
    grid.buildCheval( 15, 9);
    grid.buildCheval( 15, 19);
    grid.buildCheval( 21, 12);
    grid.buildCheval( 21, 16);
    grid.buildWater( 33, 5, 22);  // 南部护城河

    // ===== 门 =====
    grid.setTile(2, 13, "门", TileType::DOOR, "北门");
    grid.setTile(2, 14, "门", TileType::DOOR, "北门");
    grid.setTile(30, 13, "门", TileType::DOOR, "南门");
    grid.setTile(30, 14, "门", TileType::DOOR, "南门");

    // ===== 友方 NPC =====
    grid.setTile(5, 5, "粮仓", TileType::FRIEND, "粮仓");
    grid.setTile(5, 7, "粮官", TileType::FRIEND, "粮官");
    grid.setTile(6, 6, "存粮", TileType::FRIEND, "存粮");
    grid.setTile(5, 20, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(6, 21, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(14, 5, "军械", TileType::FRIEND, "军械库");
    grid.setTile(12, 14, "帅帐", TileType::FRIEND, "帅帐");
    grid.setTile(12, 22, "虞姬", TileType::FRIEND, "虞姬");
    grid.setTile(20, 5, "伤兵", TileType::FRIEND, "伤兵");
    grid.setTile(20, 6, "伤兵", TileType::FRIEND, "伤兵");
    grid.setTile(20, 7, "伤兵", TileType::FRIEND, "伤兵");
    grid.setTile(20, 19, "伙夫", TileType::FRIEND, "伙夫");
    grid.setTile(2, 6, "老兵", TileType::FRIEND, "老兵");
    grid.setTile(30, 6, "伍长", TileType::FRIEND, "伍长");
    grid.setTile(7, 11, "水井", TileType::FRIEND, "水井");
    grid.setTile(20, 15, "篝火", TileType::FRIEND, "篝火");
    grid.setTile(21, 15, "士兵", TileType::FRIEND, "士兵");
    grid.setTile(21, 16, "士兵", TileType::FRIEND, "士兵");
    grid.setTile(13, 19, "旗杆", TileType::FRIEND, "旗杆");

    // ===== 功能建筑（PHARMACY / FORGE）=====
    grid.setTile(26, 5, "军医", TileType::PHARMACY, "军医");
    grid.setTile(26, 19, "铁匠", TileType::FORGE, "铁匠");

    // ===== 物品（拾取后消失）=====
    grid.setTile(20, 12, "木炭", TileType::ITEM, "木炭");
    grid.setTile(14, 7, "楚酒", TileType::ITEM, "楚酒");
    grid.setTile(12, 24, "铜镜", TileType::ITEM, "铜镜");

    // ===== 汉军（四门）=====
    grid.setTile(1, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 22, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 22, "汉军", TileType::ENEMY, "汉军哨骑");

    // ===== 交互回调 =====
    grid.onTalk = [](const std::string& name) {
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        if (name == "虞姬")       std::cout << "虞姬：“大王，酒温好了。”" << std::endl;
        else if (name == "粮官")  std::cout << "粮官：“汉军四面围定，唯有南面防守最弱。”" << std::endl;
        else if (name == "老兵")  std::cout << "老兵：“昨夜楚歌四起，营中逃了三百人……”" << std::endl;
        else if (name == "伍长")  std::cout << "伍长：“大王突围时，务必带上末将！”" << std::endl;
        else if (name == "马夫")  std::cout << "马夫：“乌骓已经备好鞍鞯。”" << std::endl;
        else if (name == "乌骓")  std::cout << "乌骓低鸣一声，鬃毛在夜风中微动。" << std::endl;
        else if (name == "士兵")  std::cout << "士兵们围着篝火，低声唱着楚歌……" << std::endl;
        else if (name == "伤兵")  std::cout << "伤兵：“将军……我们还能回江东吗？”" << std::endl;
        else if (name == "水井")  std::cout << "井水清凉，映着半轮残月。" << std::endl;
        else if (name == "旗杆")  std::cout << "【隐藏】楚军大旗在夜风中猎猎作响。拔旗？还是留旗？" << std::endl;
        else if (name == "粮仓")  std::cout << "粮仓：存粮已不足三日。" << std::endl;
        else if (name == "存粮")  std::cout << "可分配粮草：士兵 / 战马 / 留存突围。" << std::endl;
        else if (name == "军械库") std::cout << "军械库：残破的楚军甲胄、长戈。" << std::endl;
        else if (name == "帅帐")  std::cout << "【事件】拿起太阿剑，霸王悲歌……" << std::endl;
        else if (name == "伙夫")  std::cout << "伙夫：“将军，吃口热饭再上路吧。”" << std::endl;
        else                      std::cout << "（此处无可对话内容）" << std::endl;
        console::pause();
    };

    grid.onItem = [](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        if (name == "木炭")       std::cout << "灰烬中的木炭，突围时可绘制地图。" << std::endl;
        else if (name == "楚酒")  std::cout << "半坛楚酒，饮下提士气，但会微醺。" << std::endl;
        else if (name == "铜镜")  std::cout << "虞姬的半面铜镜，似有故事。" << std::endl;
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
        std::cout << "军医：“将军负伤了？让在下看看。”" << std::endl;
        std::cout << "（治疗功能占位 —— 后续接 HP 恢复）" << std::endl;
        console::pause();
    };

    grid.onForge = [](const std::string& name) {
        console::setColor(13);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        std::cout << "铁匠：“霸王枪卷刃了？让小的打磨一番。”" << std::endl;
        std::cout << "（锻造功能占位 —— 后续接装备强化）" << std::endl;
        console::pause();
    };

    // ===== 主循环 =====
    grid.render();
    if (argc > 1) return 0;

    std::cout << "垓下营地 —— WASD 移动项羽，ESC 退出" << std::endl;
    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'w' || dir == 'a' || dir == 's' || dir == 'd') {
            grid.move(dir);
            grid.render();
        }
    }

    console::clearScreen();
    console::setColor(14);
    std::cout << "已退出。" << std::endl;
    console::setColor(7);
    return 0;
}
