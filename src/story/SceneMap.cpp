#include "SceneMap.h"
#include "QuestState.h"
#include "Game.h"
#include "SceneManager.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "ForgeManager.h"
#include "core/console.h"
#include "data/dataLoader.h"
#include "combat/combatSystem.h"
#include <iostream>
#include <memory>
#include <cctype>

namespace SceneMap {

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
    grid.buildRoom(10, 20, 14, 25, 22);
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
    grid.buildPortal(3, 13, PortalDir::Up, "营外荒郊");

    // 友方 NPC
    grid.setTile(5, 5, "粮仓", TileType::FRIEND, "粮仓");
    grid.setTile(5, 7, "粮官", TileType::FRIEND, "粮官");
    grid.setTile(6, 6, "存粮", TileType::FRIEND, "存粮");
    grid.setTile(5, 19, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(6, 21, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(14, 5, "军械", TileType::FRIEND, "军械库");
    grid.setTile(12, 14, "帅帐", TileType::ADVANCE, "帅帐");
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
    // 小卒a 对应 talk.json character_id=1
    grid.setTile(7, 14, "小卒", TileType::FRIEND, "小卒a");

    // 功能建筑
    grid.setTile(26, 5, "军医", TileType::PHARMACY, "军医");
    grid.setTile(26, 19, "铁匠", TileType::FORGE, "铁匠");

    // 物品
    grid.setTile(20, 12, "木炭", TileType::ITEM, "木炭");
    grid.setTile(14, 7, "楚酒", TileType::ITEM, "楚酒");
    grid.setTile(12, 24, "铜镜", TileType::ITEM, "铜镜");

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
    grid.setTile(11, 4,  "草药", TileType::ITEM, "草药");
    grid.setTile(11, 12, "草料", TileType::ITEM, "草料");

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
    grid.buildPortal(6, 13, PortalDir::Down, "阴陵一");

    grid.setTile(6, 5, "哨兵", TileType::FRIEND, "哨兵");
    grid.setTile(6, 22, "农人", TileType::FRIEND, "农人");
    grid.setTile(30, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(31, 5, "楚骑", TileType::FRIEND, "楚骑兵");
    grid.setTile(30, 22, "马夫", TileType::FRIEND, "马夫");
    grid.setTile(31, 22, "乌骓", TileType::FRIEND, "乌骓");
    grid.setTile(28, 13, "向导", TileType::FRIEND, "向导");
    grid.setTile(32, 13, "斥候", TileType::FRIEND, "斥候");
    grid.setTile(14, 13, "渔夫", TileType::FRIEND, "渔夫");

    grid.setTile(6, 6, "渡图", TileType::ITEM, "渡河图");
    grid.setTile(30, 6, "楚旗", TileType::ITEM, "残破楚旗");
    grid.setTile(14, 14, "蓑衣", TileType::ITEM, "蓑衣");

    grid.setTile(35, 5, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 6, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 13, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 14, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 20, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(35, 21, "汉骑", TileType::ENEMY, "汉军铁骑");
    grid.setTile(34, 8, "灌婴", TileType::ENEMY, "灌婴");

    grid.setTile(6, 22, "农舍", TileType::PHARMACY, "农舍（草药）");
    grid.setTile(30, 5, "残帐", TileType::FORGE, "残帐（修补）");

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

static void buildDongcheng(MapGrid& grid) {
    grid.setPlayer(13, 13);

    grid.buildCheval(9, 9);  grid.buildCheval(9, 17);
    grid.buildCheval(17, 9); grid.buildCheval(17, 17);
    grid.buildCheval(10, 8); grid.buildCheval(10, 18);
    grid.buildCheval(16, 8); grid.buildCheval(16, 18);

    grid.setTile(8, 13, "碎石", TileType::WALL);
    grid.setTile(18, 13, "碎石", TileType::WALL);
    grid.setTile(13, 8, "碎石", TileType::WALL);
    grid.setTile(13, 18, "碎石", TileType::WALL);

    grid.setTile(1, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 12, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(1, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(2, 8, "杨喜", TileType::ENEMY, "杨喜");
    grid.setTile(2, 18, "王翳", TileType::ENEMY, "王翳");
    grid.setTile(25, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 12, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(24, 8, "吕胜", TileType::ENEMY, "吕胜");
    grid.setTile(24, 18, "赤侯", TileType::ENEMY, "赤侯");
    grid.setTile(5, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(12, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(13, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(21, 25, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(5, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(12, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(13, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(14, 1, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(21, 1, "汉骑", TileType::ENEMY, "汉军");

    grid.setTile(2, 13, "灌婴", TileType::ENEMY, "灌婴");

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

    grid.setTile(7, 11, "赤泉", TileType::FRIEND, "赤泉侯");
    grid.setTile(7, 15, "时月", TileType::FRIEND, "秦时月");
    grid.setTile(9, 13, "钟离", TileType::FRIEND, "钟离昧");
    grid.setTile(11, 13, "二八", TileType::FRIEND, "二十八骑");

    grid.setTile(13, 11, "楚旗", TileType::ITEM, "楚军大旗");
    grid.setTile(13, 15, "太阿", TileType::ITEM, "太阿剑");
    grid.setTile(10, 13, "楚酒", TileType::ITEM, "楚酒（残）");

    grid.setTile(3, 3, "野帐", TileType::PHARMACY, "野战医帐");
    grid.setTile(3, 23, "残炉", TileType::FORGE, "野战铁炉");

    grid.setTile(26, 13, "突围", TileType::ADVANCE, "突围");
}

static void buildWujiang(MapGrid& grid) {
    grid.setPlayer(20, 13);

    grid.buildWater(1, 1, 26);
    grid.buildWater(2, 1, 26);
    grid.buildWater(3, 1, 26);

    grid.setTile(4, 12, "船埠", TileType::DOOR, "船埠");
    grid.setTile(4, 13, "船埠", TileType::DOOR, "船埠");
    grid.setTile(5, 12, "小舟", TileType::FRIEND, "乌江亭长");
    grid.setTile(5, 13, "渡船", TileType::ADVANCE, "渡船");

    grid.buildCheval(6, 5);  grid.buildCheval(6, 20);
    grid.buildCheval(8, 4);  grid.buildCheval(8, 21);
    grid.buildCheval(10, 3); grid.buildCheval(10, 22);
    grid.setTile(7, 8, "老柳", TileType::WALL);
    grid.setTile(7, 18, "老柳", TileType::WALL);
    grid.setTile(9, 10, "残碑", TileType::WALL);
    grid.setTile(9, 16, "残碑", TileType::WALL);

    grid.buildRoom(6, 11, 9, 16, 13);

    grid.setTile(18, 11, "楚骑", TileType::FRIEND, "楚骑·甲");
    grid.setTile(18, 15, "楚骑", TileType::FRIEND, "楚骑·乙");
    grid.setTile(19, 10, "楚骑", TileType::FRIEND, "楚骑·丙");
    grid.setTile(19, 16, "楚骑", TileType::FRIEND, "楚骑·丁");
    grid.setTile(21, 11, "副将", TileType::FRIEND, "副将");
    grid.setTile(21, 15, "乌骓", TileType::FRIEND, "乌骓");

    grid.setTile(7, 13, "铜镜", TileType::ITEM, "虞姬铜镜");
    grid.setTile(20, 13, "楚旗", TileType::ITEM, "楚军残旗");
    grid.setTile(22, 8, "楚酒", TileType::ITEM, "楚酒（半坛）");

    grid.setTile(25, 5, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 6, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 13, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 14, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 20, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(25, 21, "汉骑", TileType::ENEMY, "汉军");
    grid.setTile(24, 8, "吕马", TileType::ENEMY, "吕马童");
    grid.setTile(24, 18, "王翳", TileType::ENEMY, "王翳");

    grid.setTile(22, 5, "篝火", TileType::PHARMACY, "残火（疗伤）");
    grid.setTile(22, 21, "石砧", TileType::FORGE, "石砧（磨剑）");
}

MapGrid buildSceneMap(int scene_id, int) {
    MapGrid grid(37, 27);
    switch (scene_id) {
        case 1: buildGaiXia(grid); break;
        case 2: buildHuaiRiver(grid); break;
        case 3: buildDongcheng(grid); break;
        case 4: buildWujiang(grid); break;
        default: break;
    }
    return grid;
}

// 按地图名构建子地图（含进入时的落点）
static MapGrid buildNamedMap(const std::string& name, const Combatant& player) {
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

// =========================================================================
// NPC → talk.json character_id 映射
// =========================================================================

struct TalkMapping {
    const char* name;
    int character_id;
};

static int findTalkCharacterId(int scene_id, int branch_id, const std::string& name) {
    static const TalkMapping scene1[] = {
        {"小卒a", 1}, {"虞姬", 2}
    };
    static const TalkMapping scene2b1[] = {
        {"王翦", 1}, {"汉军", 2}
    };
    static const TalkMapping scene2b2[] = {
        {"副将", 1}
    };
    static const TalkMapping scene3[] = {
        {"赤泉侯", 1}, {"秦时月", 2}, {"钟离昧", 3}, {"二十八骑", 4}
    };
    static const TalkMapping scene4[] = {
        {"韩信", 1}, {"汉军", 2}, {"钟离昧", 3}, {"二十八骑", 4}, {"乌江亭长", 5}
    };

    const TalkMapping* table = nullptr;
    size_t count = 0;
    switch (scene_id) {
        case 1: table = scene1; count = sizeof(scene1)/sizeof(scene1[0]); break;
        case 2:
            if (branch_id == 1) { table = scene2b1; count = sizeof(scene2b1)/sizeof(scene2b1[0]); }
            else { table = scene2b2; count = sizeof(scene2b2)/sizeof(scene2b2[0]); }
            break;
        case 3: table = scene3; count = sizeof(scene3)/sizeof(scene3[0]); break;
        case 4: table = scene4; count = sizeof(scene4)/sizeof(scene4[0]); break;
        default: break;
    }
    if (!table) return -1;
    for (size_t i = 0; i < count; i++) {
        if (name == table[i].name) return table[i].character_id;
    }
    return -1;
}

// 未接入 talk.json 的 NPC 简单对话
static void simpleTalk(const std::string& name) {
    console::setColor(10);
    std::cout << "\n===== " << name << " =====" << std::endl;
    console::setColor(7);
    if (name == "粮仓")      std::cout << "粮仓官：“大王可有粮草分配的吩咐？”" << std::endl;
    else if (name == "存粮") std::cout << "可分配粮草：士兵 / 战马 / 留存突围。" << std::endl;
    else if (name == "伍长") std::cout << "伍长：“大王突围时，务必带上末将！”" << std::endl;
    else if (name == "马夫") std::cout << "马夫：“乌骓已经备好鞍鞯。”" << std::endl;
    else if (name == "乌骓") std::cout << "乌骓低鸣一声，鬃毛在夜风中微动。" << std::endl;
    else if (name == "士兵") std::cout << "士兵们围着篝火，低声唱着楚歌……" << std::endl;
    else if (name == "伤兵") std::cout << "伤兵：“将军……我们还能回江东吗？”" << std::endl;
    else if (name == "水井") std::cout << "井水清凉，映着半轮残月。" << std::endl;
    else if (name == "旗杆") std::cout << "楚军大旗在夜风中猎猎作响。" << std::endl;
    else if (name == "军械库")std::cout << "军械库：残破的楚军甲胄、长戈。" << std::endl;
    else if (name == "伙夫") std::cout << "伙夫：“将军，吃口热饭再上路吧。”" << std::endl;
    else if (name == "渔夫") std::cout << "渔夫：“水浅处可渡，但须趁雾，天明便来不及了。”" << std::endl;
    else if (name == "地牢渔夫") std::cout << "地牢中的渔夫：“雾里浅滩，之字而行——没有渡河图，是找不到的。”" << std::endl;
    else if (name == "向导") std::cout << "向导：“大王，过河北去，东城尚有路径。”" << std::endl;
    else if (name == "斥候") std::cout << "斥候：“汉军灌婴已追至，约三千骑！”" << std::endl;
    else if (name == "楚骑兵") std::cout << "楚骑：“愿随大王死战突围！”" << std::endl;
    else if (name == "哨兵") std::cout << "哨兵：“北岸似乎安全……但林中有异响。”" << std::endl;
    else if (name == "农人") std::cout << "农人：“军爷，拿些草药走吧。”" << std::endl;
    else if (name == "渡船") std::cout << "一叶小舟在江风中轻轻摇晃，似在催促。" << std::endl;
    else if (name == "副将") std::cout << "副将：“大王，我等誓死一战！”" << std::endl;
    else if (name.substr(0, 2) == "楚骑") std::cout << "楚骑：“赴死无悔！”" << std::endl;
    else std::cout << "（此处无可对话内容）" << std::endl;
    console::pause();
}

// =========================================================================
// 运行场景地图 WASD 交互循环（含支线子地图传送）
// =========================================================================

bool runSceneMap(Game& game, SceneManager& sm, int scene_id, int branch_id) {
    QuestState& qs = sm.getQuestState();
    Combatant& player = game.getPlayer();

    // 当前所在地图名
    std::string mapName;
    switch (scene_id) {
        case 1:  mapName = "垓下营地"; break;
        case 2:  mapName = "淮河"; break;
        case 3:  mapName = "东城"; break;
        default: mapName = "乌江"; break;
    }

    MapGrid grid = buildSceneMap(scene_id, branch_id);

    // 跑一场战斗（复用 battle_test.json），返回是否胜利
    auto runBattle = [&](const std::string& enemyName) -> bool {
        console::clearScreen();
        console::setColor(12);
        std::cout << "===== 遭遇 " << enemyName << "！战斗开始！ =====" << std::endl;
        console::setColor(7);
        console::pause();

        bool won = false;
        try {
            Battle battle = DataLoader::loadBattle("data/battle_test.json", game.getGameData());

            Combatant* p = &game.getPlayer();
            std::vector<Combatant*> companions;  // 地图遭遇战不带同伴
            std::vector<Combatant*> enemies;
            for (const auto& e : battle.enemies) enemies.push_back(e.get());

            CombatSystem combat(player, companions, enemies, battle.config, &gameData.itemPool);
            bool won = combat.startBattle();
            save.save(1, party, gameData.skillPool);

            console::clearScreen();
            if (won) {
                console::setColor(10);
                std::cout << "战斗胜利！击败了 " << name << std::endl;
            } else {
                console::setColor(12);
                std::cout << "战斗失败..." << std::endl;
            }
            console::setColor(7);
            std::cout << "按任意键返回地图" << std::endl;
            console::pause();
        } catch (const std::exception& e) {
            std::cerr << "战斗系统错误: " << e.what() << std::endl;
        }

    // ----- 物品拾取回调 -----
    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        // 拾取入包：按名称匹配物品池定义，加入角色背包
        const Item* it = findItemByName(game.getItemPool(), name);
        if (it != nullptr) {
            game.getPlayer().addItem(it->getId(), 1);
            std::cout << "「" << name << "」已放入背包。（按 B 打开背包查看）" << std::endl;
        } else {
            std::cout << "（物品池中未找到「" << name << "」的定义）" << std::endl;
        }
        console::pause();
    };

    // ----- 幕次跳转回调 -----
    grid.onAdvance = [&, scene_id](const std::string& name) {
        console::clearScreen();
        if (won) {
            console::setColor(10);
            std::cout << "战斗胜利！击败了 " << enemyName << std::endl;
        } else {
            console::setColor(12);
            std::cout << "战斗失败..." << std::endl;
        }
        console::setColor(7);
        std::cout << "按任意键返回地图" << std::endl;
        console::pause();
        return won;
    };

    // ===== 支线一：委托对话状态机 =====
    auto talkQuest = [&](const std::string& name) {
        console::clearScreen();
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);

        if (name == "老兵") {
            if (qs.q1 == 0) {
                std::cout << "老兵：“大王！昨夜又有几名弟兄趁夜逃了……\n"
                             "        他们就在营外荒郊，求大王把他们劝回来！”\n";
                std::cout << "\n【接受委托】寻回逃兵（0/3）—— 从北门白色传送门进入荒郊\n";
                qs.q1 = 1;
            } else if (qs.q1 == 1) {
                std::cout << "老兵：“逃兵们就在营外荒郊，已劝回 " << qs.deserters << "/3。”\n";
            } else if (qs.q1 == 2) {
                std::cout << "老兵：“三名弟兄都回来了！多谢大王！\n";
                game.getGold() += 30;
                player.addItem("herb_potion", 1);
                qs.morale += 15;
                std::cout << "【奖励】金币 +30、伤药 ×1，军心 +15（当前军心 " << qs.morale << "）\n";
                qs.q1 = 3;
            } else {
                std::cout << "老兵：“军心已定，愿随大王死战！”\n";
            }
            console::pause();
            return;
        }

        if (name == "粮官") {
            if (qs.q2 == 0) {
                std::cout << "粮官：“南门最近有汉军哨骑出没，请大王巡视南门、将其击退！”\n";
                std::cout << "\n【接受委托】巡南门 —— 出南门击败汉军哨骑\n";
                qs.q2 = 1;
            } else if (qs.q2 == 1) {
                std::cout << "粮官：“南门外的汉军哨骑仍在游荡，请大王出手！”\n";
            } else if (qs.q2 == 2) {
                std::cout << "粮官：“哨骑已退！这是赏格，请大王收下。\n";
                game.getGold() += 20;
                player.addItem("iron_sword", 1);
                qs.morale += 10;
                std::cout << "【奖励】金币 +20、铁剑 ×1，军心 +10（当前军心 " << qs.morale << "）\n";
                qs.q2 = 3;
            } else {
                std::cout << "粮官：“南门安宁，全赖大王。”\n";
            }
            console::pause();
            return;
        }

        if (name == "粮仓") {
            if (qs.q3 == 0) {
                std::cout << "粮仓官：“仓中余粮无多，请大王示下如何分配？”\n\n"
                             "  1. 优先士兵 —— 伤药×2，军心 +5\n"
                             "  2. 优先战马 —— 草料×2，军心不变\n"
                             "  3. 留存突围 —— 木炭×2，军心 -5\n"
                             "\n请输入 1 / 2 / 3：";
                int key = console::readKey();
                if (key == '1') {
                    player.addItem("herb_potion", 2);
                    qs.morale += 5; qs.q3choice = 1;
                    std::cout << "\n【分配】粮草优先士兵，营中士气大振。\n";
                } else if (key == '2') {
                    player.addItem("fodder", 2);
                    qs.q3choice = 2;
                    std::cout << "\n【分配】粮草优先战马，马匹膘壮。\n";
                } else if (key == '3') {
                    player.addItem("charcoal", 2);
                    qs.morale -= 5; qs.q3choice = 3;
                    std::cout << "\n【分配】粮草留存突围之用，士兵们略有微词。\n";
                } else {
                    std::cout << "\n（未作分配）\n";
                    console::pause();
                    return;
                }
                std::cout << "当前军心 " << qs.morale << "\n";
                qs.q3 = 3;
            } else {
                std::cout << "粮仓官：“粮草已按大王的意思分派妥当。”\n";
            }
            console::pause();
            return;
        }
    };

    // 荒郊逃兵对话
    auto talkDeserter = [&](const std::string& name) {
        console::clearScreen();
        console::setColor(10);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);
        int idx = (name == "逃兵甲") ? 0 : (name == "逃兵乙" ? 1 : 2);
        if (!qs.deserterTalked[idx]) {
            qs.deserterTalked[idx] = true;
            qs.deserters++;
            if (idx == 0)      std::cout << "逃兵甲：“楚歌四起，弟兄们都以为家乡尽失……大王既亲来相劝，我跟您回去！”\n";
            else if (idx == 1) std::cout << "逃兵乙：“家中老母尚在……但大王不弃，我愿再持干戈！”\n";
            else               std::cout << "逃兵丙：“我等糊涂，听了一夜楚歌便散了军心——大王，走！”\n";
            std::cout << "\n已劝回逃兵 " << qs.deserters << "/3\n";
            if (qs.deserters >= 3 && qs.q1 == 1) {
                qs.q1 = 2;
                console::setColor(14);
                std::cout << "三名逃兵均已回心转意，回营地老兵处复命吧！\n";
                console::setColor(7);
            }
        } else {
            std::cout << "“大王，我这就随您回营。”\n";
        }
        console::pause();
    };

    // ===== 支线二：田夫 =====
    auto talkTianfu = [&]() {
        console::clearScreen();
        console::setColor(10);
        std::cout << "\n===== 田夫 =====" << std::endl;
        console::setColor(7);
        if (!qs.yinlingUnlocked) {
            std::cout << "田夫：“将军不知，此去北岸有一条阴陵古道，\n"
                         "        迷雾深处连着古渡，只是从没人走通过……\n"
                         "        若要寻浅滩，需先有渡河图；寒水刺骨，还须蓑衣护体。”\n";
            qs.yinlingUnlocked = true;
            console::setColor(14);
            std::cout << "\n阴陵古道入口已在北岸显现（白色传送门 ↓）。\n";
            console::setColor(7);
        } else {
            std::cout << "田夫：“阴陵古道凶险，将军保重。雾中浅滩需凭渡河图辨认。”\n";
        }
        console::pause();
    };

    // ===== 给当前地图挂接回调 =====
    auto configure = [&](const std::string& mn) {
        // 对话
        grid.onTalk = [&, mn](const std::string& name) {
            // 支线一·营地委托
            if (mn == "垓下营地" && (name == "老兵" || name == "粮官" || name == "粮仓")) {
                talkQuest(name);
                return;
            }
            // 支线一·荒郊逃兵
            if (mn == "营外荒郊" && name.compare(0, 2, "逃兵") == 0) {
                talkDeserter(name);
                return;
            }
            // 支线二·田夫
            if (mn == "淮河" && name == "田夫") {
                talkTianfu();
                return;
            }
            int cid = findTalkCharacterId(scene_id, branch_id, name);
            if (cid > 0) sm.getTalkManager().talkCharacterExternal(scene_id, cid, branch_id);
            else simpleTalk(name);
        };

        // 药店
        grid.onPharmacy = [&](const std::string& name) {
            console::clearScreen();
            console::setColor(11);
            std::cout << "===== " << name << " =====" << std::endl;
            console::setColor(7);
            sm.enterPharmacy(game);
        };

        // 锻造
        grid.onForge = [&](const std::string& name) {
            console::clearScreen();
            console::setColor(13);
            std::cout << "===== " << name << " =====" << std::endl;
            console::setColor(7);
            sm.enterForge();
        };

        // 战斗
        grid.onBattle = [&, mn](const std::string& name) {
            bool won = runBattle(name);
            // 委托二：南门外哨骑
            if (won && mn == "垓下营地" && qs.q2 == 1 && name == "汉军哨骑") {
                qs.q2 = 2;
                console::setColor(14);
                std::cout << "南门哨骑已肃清，回粮官处复命吧！\n";
                console::setColor(7);
                console::pause();
            }
            // 支线二：灌婴 BOSS
            if (won && mn == "阴陵三" && name == "灌婴") {
                qs.guanyingDefeated = true;
                console::setColor(14);
                std::cout << "灌婴败退！古渡返回之路已开启（上方白色传送门）。\n";
                console::setColor(7);
                console::pause();
            }
        };

        // 拾取（中文名 → 实际物品 id）
        grid.onItem = [&](const std::string& name) {
            std::string itemId;
            if (name == "渡河图") itemId = "hr_map";
            else if (name == "蓑衣") itemId = "hr_raincoat";
            else if (name == "草药") itemId = "herb";
            else if (name == "草料") itemId = "fodder";

            if (!itemId.empty()) player.addItem(itemId, 1);
            console::setColor(14);
            std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
            console::setColor(7);
            console::pause();
        };

        // 幕次跳转
        grid.onAdvance = [&, mn, scene_id](const std::string& name) {
            console::clearScreen();
            console::setColor(13);
            std::cout << "\n===== " << name << " =====" << std::endl;
            console::setColor(7);

            const char* prompts[] = {
                nullptr,
                "项羽立于帅帐之中，拔剑四顾。\n"
                "营外楚歌四起，将士离散，军心已溃。\n"
                "「此天亡楚也，非战之罪！」\n"
                "霸王决意率八百骑趁夜突围，南走淮河……\n",
                "渡过淮河，身后追兵渐远。\n"
                "灌婴三千铁骑紧追不舍，项王仅余百余骑。\n"
                "一路东行，东城在望——\n"
                "那便是霸王最后的战场。\n",
                "东城一战，二十八骑杀穿汉军重围。\n"
                "项王仰天大笑：「今日固死，然愿快战三合！」\n"
                "残兵南下，乌江在前——\n"
                "江东子弟何在？天之亡我，何渡为！\n",
                "乌江之畔，亭长泊舟以待。\n"
                "「江东虽小，地方千里，众数十万，亦足王也。愿大王急渡！」\n"
                "项王笑曰：「天之亡我，我何渡为！」\n"
            };

            if (scene_id < 4) {
                sm.printWords(prompts[scene_id], 11, 500, 80);
                // 委托三的选择影响一句旁白
                if (scene_id == 1) {
                    if (qs.q3choice == 1)
                        sm.printWords("营中士卒因多分粮草，突围时多有死战之士。", 14, 300, 60);
                    else if (qs.q3choice == 2)
                        sm.printWords("战马食饱，乌骓嘶鸣，行军快了几分。", 14, 300, 60);
                    else if (qs.q3choice == 3)
                        sm.printWords("留存的粮草支撑着这一路奔逃。", 14, 300, 60);
                }
                console::setColor(14);
                std::cout << "\n是否进入第 " << (scene_id + 1) << " 幕？（Y 确认 / N 留在当前场景）" << std::endl;
                console::setColor(7);
                int key = console::readKey();
                if (key == 'y' || key == 'Y') {
                    grid.advanceTriggered = true;
                    sm.changeScene(scene_id + 1);
                    sm.ShowBackground(scene_id + 1);
                }
            } else {
                sm.printWords(prompts[4], 12, 500, 80);
                console::setColor(14);
                std::cout << "\n是否渡江？（Y 确认 / N 留在江畔）" << std::endl;
                console::setColor(7);
                int key = console::readKey();
                if (key == 'y' || key == 'Y') {
                    grid.advanceTriggered = true;
                    sm.ShowBackground(4);
                }
            }
        };

        // 传送门
        grid.onPortal = [&, mn](const std::string& target) {
            // 支线二：阴陵入口需田夫指路
            if (mn == "淮河" && target == "阴陵一" && !qs.yinlingUnlocked) {
                grid.portalTriggered = false;
                console::setColor(14);
                std::cout << "\n荒草丛生，似乎无路……先找附近的「田夫」问问吧。" << std::endl;
                console::setColor(7);
                console::pause();
                return;
            }
            // 支线二：三层返回门需先击败灌婴
            if (mn == "阴陵三" && target == "淮河" && !qs.guanyingDefeated) {
                grid.portalTriggered = false;
                console::setColor(12);
                std::cout << "\n古渡被汉军封锁——先击败深处的灌婴！" << std::endl;
                console::setColor(7);
                console::pause();
                return;
            }
            // 支线二：下三层时蓑衣检查
            if (mn == "阴陵二" && target == "阴陵三" &&
                player.getEquippedItemId(0) != "hr_raincoat" && !qs.raincoatWarned) {
                qs.raincoatWarned = true;
                player.takeDamage(10);
                console::setColor(12);
                std::cout << "\n寒水刺骨，项羽损失 10 点生命（装备蓑衣可免）。当前 HP "
                          << player.getHP() << std::endl;
                console::setColor(7);
                console::pause();
            }
            console::setColor(15);
            std::cout << "\n传送 → " << target << std::endl;
            console::setColor(7);
            console::pause();
        };
    };

    // ===== WASD 主循环 =====
    configure(mapName);
    grid.render();
    std::cout << "第" << scene_id << "幕 · " << mapName
              << " —— WASD 移动，白色箭头=传送门，ESC 退出场景" << std::endl;

    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir != 'w' && dir != 'a' && dir != 's' && dir != 'd') continue;

        grid.move(dir);

        // 传送门：切换子地图
        if (grid.portalTriggered) {
            std::string target = grid.portalTarget;
            grid.portalTriggered = false;
            grid.portalTarget.clear();
            grid.advanceTriggered = false;
            mapName = target;
            // 进入二层重置蓑衣提示
            if (target == "阴陵二") qs.raincoatWarned = false;
            grid = buildNamedMap(target, player);
            configure(mapName);
            grid.render();
            std::cout << "当前位置：" << mapName << std::endl;
            continue;
        }

        // Y 确认跳转下一幕 → 退出当前场景
        if (grid.advanceTriggered) break;

        grid.render();
    }

    console::clearScreen();
    return true;
}

} // namespace SceneMap
