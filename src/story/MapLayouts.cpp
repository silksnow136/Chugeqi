#include "MapLayouts.h"

namespace MapLayouts {

// =========================================================================
// 四幕主线地图 + 支线子地图布局
// =========================================================================

static void buildGaiXia(MapGrid& grid) {
    grid.setPlayer(13, 14);

    // 房间
    grid.buildRoom( 4, 3, 9, 10, 6);
    grid.buildRoom( 4, 17, 9, 24, 20);
    grid.buildRoom(13, 3, 16, 8, 5);
    grid.buildRoom(10, 11, 17, 18, 14);
    grid.buildRoom(11, 20, 15, 25, 22);
    grid.buildRoom(19, 3, 23, 10, 6);
    grid.buildRoom(19, 17, 22, 23, 19);
    grid.buildRoom(25, 3, 28, 8, 5);
    grid.buildRoom(25, 17, 28, 23, 19);

    // 栅栏
    grid.buildFence( 2, 3, 24, 13, 14);
    grid.buildFence(30, 3, 24, 13, 14);

    // 拒马
    grid.buildCheval( 8, 12); grid.buildCheval( 8, 15);
    grid.buildCheval(15,  9); grid.buildCheval(15, 19);
    grid.buildCheval(21, 12); grid.buildCheval(21, 16);

    // 护城河
    grid.buildWater(33, 5, 22);

    // 门
    grid.setTile(2, 13, "门", TileType::DOOR, "北门");
    grid.setTile(2, 14, "门", TileType::DOOR, "北门");
    grid.setTile(30, 13, "门", TileType::DOOR, "南门");
    grid.setTile(30, 14, "门", TileType::DOOR, "南门");

    // 【支线一】北门内侧传送门 → 营外荒郊
    grid.buildPortal(3, 14, PortalDir::Up, "营外荒郊");

    // 友方 NPC
    grid.setTile(5, 5, "粮仓", TileType::FRIEND, "粮仓");
    grid.setTile(5, 7, "粮官", TileType::FRIEND, "粮官");
    grid.setTile(6, 6, "存粮", TileType::FRIEND, "存粮");
    grid.setTile(5, 19, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(6, 21, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(14, 5, "军械", TileType::FRIEND, "军械库");
    grid.setTile(12, 14, "帅帐", TileType::ADVANCE, "帅帐");
    grid.setTile(13, 22, "虞姬", TileType::FRIEND, "虞姬");
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
    // 小卒a 对应 talk.json character_id=1
    grid.setTile(7, 14, "小卒", TileType::FRIEND, "小卒a");

    // 功能建筑
    grid.setTile(26, 5, "军医", TileType::PHARMACY, "军医");

    // 物品
    grid.setTile(20, 12, "木炭", TileType::ITEM, "charcoal");
    grid.setTile(14, 7, "楚酒", TileType::ITEM, "chu_wine");
    grid.setTile(12, 24, "铜镜", TileType::ITEM, "bronze_mirror");

    // 汉军（南门外哨骑 —— 委托二的战斗目标）
    grid.setTile(1, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 22, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 13, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 14, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 22, "汉军", TileType::ENEMY, "汉军哨骑");
}

// 【支线一】营外荒郊（15×17）
static void buildWild(MapGrid& grid) {
    grid.setPlayer(3, 8);

    // 返回垓下营地的传送门（顶部，箭头↓）
    grid.buildPortal(2, 8, PortalDir::Down, "垓下营地");

    // 破庙
    grid.buildRoom(3, 7, 6, 10, 8);
    // 枯木
    grid.buildCheval(4, 5);
    grid.buildCheval(4, 11);
    // 南边长溪
    grid.buildWater(13, 3, 11);

    // 三名逃兵（委托一）
    grid.setTile(7, 5,  "逃兵", TileType::FRIEND, "逃兵甲");
    grid.setTile(7, 9,  "逃兵", TileType::FRIEND, "逃兵乙");
    grid.setTile(7, 13, "逃兵", TileType::FRIEND, "逃兵丙");

    // 拾取物
    grid.setTile(11, 4,  "草药", TileType::ITEM, "herb");
    grid.setTile(11, 12, "草料", TileType::ITEM, "fodder");

    // 巡逻汉军
    grid.setTile(9, 15, "汉军", TileType::ENEMY, "汉军哨骑");
}

static void buildHuaiRiver(MapGrid& grid) {
    grid.setPlayer(30, 13);

    grid.buildWater(17, 1, 6);
    grid.buildWater(17, 20, 6);
    grid.buildWater(18, 1, 5);
    grid.buildWater(18, 21, 5);
    grid.buildWater(19, 1, 4);
    grid.buildWater(19, 22, 4);

    grid.buildCheval(5, 8);  grid.buildCheval(5, 18);
    grid.buildCheval(8, 6);  grid.buildCheval(8, 20);
    grid.buildCheval(12, 10); grid.buildCheval(12, 16);

    grid.buildRoom(3, 3, 8, 8, 5);
    grid.buildRoom(3, 19, 8, 24, 22);
    grid.buildRoom(28, 3, 33, 8, 5);
    grid.buildRoom(28, 19, 33, 24, 22);

    grid.buildWater(16, 1, 26);
    grid.buildWater(20, 1, 26);

    // 浅滩渡口：必须在 buildWater 之后设置，否则会被水墙覆盖
    grid.setTile(16, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(16, 13, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(17, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(17, 13, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(18, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(18, 13, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(19, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(19, 13, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(20, 12, "浅滩", TileType::DOOR, "浅滩");
    grid.setTile(20, 13, "浅滩", TileType::DOOR, "浅滩");

    // 【支线二】田夫指路 + 阴陵古道入口（白色传送门）
    grid.setTile(6, 9, "田夫", TileType::FRIEND, "田夫");
    grid.buildPortal(6, 13, PortalDir::Up, "阴陵一");

    grid.setTile(6, 5, "哨兵", TileType::FRIEND, "哨兵");
    grid.setTile(6, 22, "农人", TileType::FRIEND, "农人");
    grid.setTile(30, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(31, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(30, 22, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(31, 22, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(28, 13, "向导", TileType::FRIEND, "向导");
    grid.setTile(32, 13, "斥候", TileType::FRIEND, "斥候");
    grid.setTile(14, 13, "渔夫", TileType::FRIEND, "渔夫");

    grid.setTile(6, 6, "渡图", TileType::ITEM, "hr_map");
    grid.setTile(30, 6, "楚旗", TileType::ITEM, "hr_banner");
    grid.setTile(14, 14, "蓑衣", TileType::ITEM, "hr_raincoat");

    grid.setTile(35, 5, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 6, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 13, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 14, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 20, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 21, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(34, 8, "灌婴", TileType::ENEMY, "灌婴");

    grid.setTile(6, 22, "农舍", TileType::PHARMACY, "农舍（草药）");

    // 跳转点：北渡淮河后前往东城
    grid.setTile(15, 12, "北渡", TileType::ADVANCE, "北渡");
}

// 【支线二】阴陵一层 · 沼泽外围（15×17）
static void buildYinlingFloor1(MapGrid& grid) {
    grid.setPlayer(3, 8);
    grid.buildPortal(2, 8, PortalDir::Up, "淮河");       // 返回
    grid.buildPortal(12, 8, PortalDir::Down, "阴陵二");  // 深入

    grid.buildWater(5, 4, 4);
    grid.buildWater(6, 10, 4);
    grid.buildCheval(7, 5);
    grid.buildCheval(7, 11);

    grid.setTile(9, 6,  "汉军", TileType::ENEMY, "汉军斥候");
    grid.setTile(9, 10, "汉军", TileType::ENEMY, "汉军斥候");
}

// 【支线二】阴陵二层 · 迷雾水域（15×17）
static void buildYinlingFloor2(MapGrid& grid, const Combatant& player) {
    grid.setPlayer(3, 8);
    grid.buildPortal(2, 8, PortalDir::Up, "阴陵一");
    grid.buildPortal(12, 8, PortalDir::Down, "阴陵三");

    // 三条横河（急流）
    grid.buildWater(5, 3, 11);
    grid.buildWater(8, 3, 11);
    grid.buildWater(11, 3, 11);

    // 之字形浅滩：持有「渡河图」才能看见（可通行），否则只是水域
    if (player.hasItem("hr_map")) {
        grid.setTile(5, 5,  "浅滩", TileType::DOOR, "浅滩");
        grid.setTile(8, 12, "浅滩", TileType::DOOR, "浅滩");
        grid.setTile(11, 5, "浅滩", TileType::DOOR, "浅滩");
    }

    grid.setTile(3, 14, "渔夫", TileType::FRIEND, "地牢渔夫");
}

// 【支线二】阴陵三层 · 古渡 BOSS（15×17）
static void buildYinlingFloor3(MapGrid& grid) {
    grid.setPlayer(3, 8);
    // 返回淮河的传送门（击败灌婴后才生效）
    grid.buildPortal(2, 8, PortalDir::Up, "淮河");

    grid.buildRoom(3, 7, 6, 10, 8);   // 古渡亭
    grid.buildWater(12, 3, 11);       // 江面

    // BOSS
    grid.setTile(8, 8, "灌婴", TileType::ENEMY, "灌婴");
}

// 第三幕：东城快战（限制探索范围的战场）
static void buildDongcheng(MapGrid& grid) {
    grid.setPlayer(13, 13);

    // 战场围栏（墙圈出活动区，限制探索范围）
    grid.buildRoom(8, 6, 21, 20, 13);   // 门在 (8,13) 和 (21,13)

    // 拒马
    grid.buildCheval(10, 10); grid.buildCheval(10, 16);
    grid.buildCheval(18, 10); grid.buildCheval(18, 16);

    // 汉军骑兵（战场内的遭遇战）
    grid.setTile(9, 9, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(9, 17, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(19, 9, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(19, 17, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 7, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 19, "汉骑", TileType::ENEMY, "汉军");

    // 友方
    grid.setTile(11, 13, "二八", TileType::FRIEND, "二十八骑");
    grid.setTile(16, 13, "钟离", TileType::FRIEND, "钟离昧");
    grid.setTile(12, 16, "副将", TileType::FRIEND, "副将");

    // 野战医帐
    grid.setTile(17, 13, "野帐", TileType::PHARMACY, "野战医帐");

    // 突围（触发乌江结局）
    grid.setTile(20, 13, "突围", TileType::ADVANCE, "突围");
}

MapGrid buildSceneMap(int scene_id, int) {
    MapGrid grid(37, 27);
    switch (scene_id) {
        case 1: buildGaiXia(grid); break;
        case 2: buildHuaiRiver(grid); break;
        case 3: buildDongcheng(grid); break;
        default: break;
    }
    return grid;
}

// 按地图名构建子地图（含进入时的落点）
MapGrid buildNamedMap(const std::string& name, const Combatant& player) {
    if (name == "营外荒郊") {
        MapGrid g(15, 17);
        buildWild(g);
        g.setPlayer(3, 8);
        return g;
    }
    if (name == "阴陵一") {
        MapGrid g(15, 17);
        buildYinlingFloor1(g);
        g.setPlayer(3, 8);
        return g;
    }
    if (name == "阴陵二") {
        MapGrid g(15, 17);
        buildYinlingFloor2(g, player);
        g.setPlayer(3, 8);
        return g;
    }
    if (name == "阴陵三") {
        MapGrid g(15, 17);
        buildYinlingFloor3(g);
        g.setPlayer(3, 8);
        return g;
    }
    if (name == "垓下营地") {
        MapGrid g(37, 27);
        buildGaiXia(g);
        g.setPlayer(4, 13);  // 落在北门传送门内侧
        return g;
    }
    if (name == "淮河") {
        MapGrid g(37, 27);
        buildHuaiRiver(g);
        g.setPlayer(7, 13);  // 落在阴陵入口传送门内侧
        return g;
    }
    // 兜底
    MapGrid g(37, 27);
    buildGaiXia(g);
    return g;
}

} // namespace MapLayouts
