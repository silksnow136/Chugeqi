#include "SceneMap.h"
#include "Game.h"
#include "SceneManager.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "ForgeManager.h"
#include "core/console.h"
#include "core/dataLoader.h"
#include "combat/combatSystem.h"
#include "combat/saveManager.h"
#include <iostream>
#include <memory>

namespace SceneMap {

// =========================================================================
// 四幕地图布局构建
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

    // 汉军
    grid.setTile(1, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(1, 22, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 5, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 6, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 21, "汉军", TileType::ENEMY, "汉军哨骑");
    grid.setTile(31, 22, "汉军", TileType::ENEMY, "汉军哨骑");
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
    // 东/西
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

    // 二十八骑
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

    // 对话人物（talk.json）
    grid.setTile(7, 11, "赤泉", TileType::FRIEND, "赤泉侯");
    grid.setTile(7, 15, "时月", TileType::FRIEND, "秦时月");
    grid.setTile(9, 13, "钟离", TileType::FRIEND, "钟离昧");
    grid.setTile(11, 13, "二八", TileType::FRIEND, "二十八骑");

    grid.setTile(13, 11, "楚旗", TileType::ITEM, "楚军大旗");
    grid.setTile(13, 15, "太阿", TileType::ITEM, "太阿剑");
    grid.setTile(10, 13, "楚酒", TileType::ITEM, "楚酒（残）");

    grid.setTile(3, 3, "野帐", TileType::PHARMACY, "野战医帐");
    grid.setTile(3, 23, "残炉", TileType::FORGE, "野战铁炉");

    // 跳转点：突围南下前往乌江
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

MapGrid buildSceneMap(int scene_id, int branch_id) {
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

// =========================================================================
// NPC → talk.json character_id 映射
// =========================================================================

struct TalkMapping {
    const char* name;
    int character_id;
};

// 按场景 id 查 NPC 对应的 character_id；未找到返回 -1
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
    // 通用占位文本
    if (name == "粮仓")      std::cout << "粮仓：存粮已不足三日。" << std::endl;
    else if (name == "粮官") std::cout << "粮官：“汉军四面围定，唯有南面防守最弱。”" << std::endl;
    else if (name == "老兵") std::cout << "老兵：“昨夜楚歌四起，营中逃了三百人……”" << std::endl;
    else if (name == "伍长") std::cout << "伍长：“大王突围时，务必带上末将！”" << std::endl;
    else if (name == "马夫") std::cout << "马夫：“乌骓已经备好鞍鞯。”" << std::endl;
    else if (name == "乌骓") std::cout << "乌骓低鸣一声，鬃毛在夜风中微动。" << std::endl;
    else if (name == "士兵") std::cout << "士兵们围着篝火，低声唱着楚歌……" << std::endl;
    else if (name == "伤兵") std::cout << "伤兵：“将军……我们还能回江东吗？”" << std::endl;
    else if (name == "水井") std::cout << "井水清凉，映着半轮残月。" << std::endl;
    else if (name == "旗杆") std::cout << "楚军大旗在夜风中猎猎作响。" << std::endl;
    else if (name == "存粮") std::cout << "可分配粮草：士兵 / 战马 / 留存突围。" << std::endl;
    else if (name == "军械库")std::cout << "军械库：残破的楚军甲胄、长戈。" << std::endl;
    else if (name == "帅帐") std::cout << "【事件】拿起太阿剑，霸王悲歌……" << std::endl;
    else if (name == "伙夫") std::cout << "伙夫：“将军，吃口热饭再上路吧。”" << std::endl;
    else if (name == "渔夫") std::cout << "渔夫：“水浅处可渡，但须趁雾，天明便来不及了。”" << std::endl;
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
// 运行场景地图 WASD 交互循环
// =========================================================================

bool runSceneMap(Game& game, SceneManager& sm, int scene_id, int branch_id) {
    MapGrid grid = buildSceneMap(scene_id, branch_id);

    // ----- 对话回调 -----
    grid.onTalk = [&](const std::string& name) {
        int cid = findTalkCharacterId(scene_id, branch_id, name);
        if (cid > 0) {
            // 接入 talk.json 对话系统
            sm.getTalkManager().talkCharacterExternal(scene_id, cid, branch_id);
        } else {
            simpleTalk(name);
        }
    };

    // ----- 药店回调 -----
    grid.onPharmacy = [&](const std::string& name) {
        console::clearScreen();
        console::setColor(11);
        std::cout << "===== " << name << " =====" << std::endl;
        console::setColor(7);
        // 复用 SceneManager 中的 PharManager
        sm.enterPharmacy(game);
    };

    // ----- 锻造回调 -----
    grid.onForge = [&](const std::string& name) {
        console::clearScreen();
        console::setColor(13);
        std::cout << "===== " << name << " =====" << std::endl;
        console::setColor(7);
        sm.enterForge();
    };

    // ----- 战斗回调 -----
    grid.onBattle = [&](const std::string& name) {
        console::clearScreen();
        console::setColor(12);
        std::cout << "===== 遭遇 " << name << "！战斗开始！ =====" << std::endl;
        console::setColor(7);
        console::pause();

        try {
            GameData gameData = DataLoader::loadGameData("data/");
            SaveManager save("save.db");
            std::vector<std::unique_ptr<Combatant>> party = save.loadParty(gameData.skillPool);
            if (party.empty()) {
                party = DataLoader::loadPartyTemplates("data/battle_test.json", gameData.skillPool);
                save.saveParty(party, gameData.skillPool);
            }
            Battle battle = DataLoader::loadBattle("data/battle_test.json", gameData);

            Combatant* player = party[0].get();
            std::vector<Combatant*> companions;
            for (size_t i = 1; i < party.size(); i++) companions.push_back(party[i].get());
            std::vector<Combatant*> enemies;
            for (const auto& e : battle.enemies) enemies.push_back(e.get());

            CombatSystem combat(player, companions, enemies, battle.config);
            bool won = combat.startBattle();
            save.saveParty(party, gameData.skillPool);

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
            console::pause();
        }
    };

    // ----- 物品拾取回调 -----
    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        console::pause();
    };

    // ----- 幕次跳转回调 -----
    grid.onAdvance = [&, scene_id](const std::string& name) {
        console::clearScreen();
        console::setColor(13);
        std::cout << "\n===== " << name << " =====" << std::endl;
        console::setColor(7);

        // 各幕跳转提示词
        const char* prompts[] = {
            nullptr,  // scene 0 不存在
            // 第一幕 → 第二幕
            "项羽立于帅帐之中，拔剑四顾。\n"
            "营外楚歌四起，将士离散，军心已溃。\n"
            "「此天亡楚也，非战之罪！」\n"
            "霸王决意率八百骑趁夜突围，南走淮河……\n",
            // 第二幕 → 第三幕
            "渡过淮河，身后追兵渐远。\n"
            "灌婴三千铁骑紧追不舍，项王仅余百余骑。\n"
            "一路东行，东城在望——\n"
            "那便是霸王最后的战场。\n",
            // 第三幕 → 第四幕
            "东城一战，二十八骑杀穿汉军重围。\n"
            "项王仰天大笑：「今日固死，然愿快战三合！」\n"
            "残兵南下，乌江在前——\n"
            "江东子弟何在？天之亡我，何渡为！\n",
            // 第四幕结局
            "乌江之畔，亭长泊舟以待。\n"
            "「江东虽小，地方千里，众数十万，亦足王也。愿大王急渡！」\n"
            "项王笑曰：「天之亡我，我何渡为！」\n"
        };

        if (scene_id < 4) {
            // 显示过渡旁白 + 确认提示
            sm.printWords(prompts[scene_id], 11, 500, 80);
            console::setColor(14);
            std::cout << "\n是否进入第 " << (scene_id + 1) << " 幕？（Y 确认 / N 留在当前场景）" << std::endl;
            console::setColor(7);
            int key = console::readKey();
            if (key == 'y' || key == 'Y') {
                grid.advanceTriggered = true;  // 确认跳转 → 退出 WASD 循环
                sm.changeScene(scene_id + 1);
                sm.ShowBackground(scene_id + 1);
            } else {
                // 选择留下，重绘地图继续游戏
                grid.requestClearRender();
                grid.render();
            }
        } else {
            // 第四幕渡船 → 触发结局
            sm.printWords(prompts[4], 12, 500, 80);
            console::setColor(14);
            std::cout << "\n是否渡江？（Y 确认 / N 留在江畔）" << std::endl;
            console::setColor(7);
            int key = console::readKey();
            if (key == 'y' || key == 'Y') {
                grid.advanceTriggered = true;
                sm.ShowBackground(4);
            } else {
                grid.requestClearRender();
                grid.render();
            }
        }
    };

    // ----- WASD 主循环 -----
    grid.render();
    std::cout << "第" << scene_id << "幕 —— WASD 移动项羽，ESC 退出场景" << std::endl;

    while (true) {
        int key = console::readKey();
        if (key == 27) break;  // ESC 退出场景
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'w' || dir == 'a' || dir == 's' || dir == 'd') {
            grid.move(dir);
            // 只有 Y 确认跳转后 advanceTriggered 才为 true → 退出循环
            if (grid.advanceTriggered) break;
            grid.render();
        }
    }

    console::clearScreen();
    return true;
}

} // namespace SceneMap
