// ---------------------------------------------------------------------------
// gaiXiaMap.cpp — 第一幕「垓下营地」地图效果展示
//
// 37×27 网格。装饰墙拟物：城墙 █、帐墙 =、栅栏 ║、拒马 ╳、护城河 ~。
// 房间内部留足通道（门内无遮挡），布置 NPC / 物品 / 功能建筑 / 汉军。
// WASD 移动项羽，ESC 退出；带任意参数只渲染一次。
// ---------------------------------------------------------------------------

#include "story/MapGrid.h"
#include "core/console.h"
#include "combat/character.h"
#include "combat/item.h"
#include "core/dataLoader.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cctype>

// ---------------------------------------------------------------------------
// 背包/物品辅助
// ---------------------------------------------------------------------------
static const char* kSlotNames[4] = { "防具", "武器", "鞋子", "配饰" };

// 按名称在物品池中查找物品（地图物品格用中文名标注）
const Item* findItemByName(const ItemPool& pool, const std::string& name) {
    for (const auto& kv : pool) {
        if (kv.second->getName() == name) return kv.second.get();
    }
    return nullptr;
}

// 读取一行命令（逐字符读取并回显；回车结束，ESC 取消）
std::string readCommandLine() {
    std::string cmd;
    while (true) {
        int k = console::readKey();
        if (k == 13 || k == 10) { std::cout << std::endl; break; }   // 回车
        if (k == 27) { std::cout << std::endl; return ""; }          // ESC 取消
        if (k == 8) { if (!cmd.empty()) { cmd.pop_back(); std::cout << "\b \b"; } } // 退格
        else if (k >= 32 && k < 127) { cmd += static_cast<char>(k); std::cout << static_cast<char>(k); }
    }
    return cmd;
}

// 背包条目（分组显示用）
struct BagEntry {
    std::string itemId;
    std::string name;
    int count;
    std::string category; // equipment / potion / material
    int slot;             // 装备槽位，非装备为 -1
};

// 分类排序权重：装备0 药水1 材料2
int categoryRank(const std::string& c) {
    if (c == "equipment") return 0;
    if (c == "potion") return 1;
    return 2;
}

// 收集背包条目并按 装备->药水->材料 排序
void collectBagEntries(Combatant* player, const ItemPool& pool, std::vector<BagEntry>& out) {
    const auto& inv = player->getInventory();
    for (const auto& kv : inv) {
        if (kv.second <= 0) continue;
        BagEntry e;
        e.itemId = kv.first;
        e.count = kv.second;
        auto it = pool.find(kv.first);
        if (it != pool.end()) {
            e.name = it->second->getName();
            e.category = it->second->getCategory();
            const Equipment* eq = dynamic_cast<const Equipment*>(it->second.get());
            e.slot = eq ? static_cast<int>(eq->getSlot()) : -1;
        } else {
            e.name = kv.first;
            e.category = "material";
            e.slot = -1;
        }
        out.push_back(e);
    }
    std::stable_sort(out.begin(), out.end(), [](const BagEntry& a, const BagEntry& b) {
        return categoryRank(a.category) < categoryRank(b.category);
    });
}

// 背包界面：显示当前装备与分类物品清单；equip+编号 装配 / use+编号 使用药水；回车或ESC返回地图
void showBackpack(Combatant* player, const ItemPool& pool) {
    while (true) {
        console::clearScreen();
        std::cout << "========== 背包 ==========\n\n";

        // 当前装备
        std::cout << "【当前装备】\n";
        for (int s = 0; s < 4; s++) {
            const std::string& id = player->getEquippedItemId(s);
            if (id.empty()) std::cout << "  " << kSlotNames[s] << "：(无)\n";
            else {
                auto it = pool.find(id);
                std::cout << "  " << kSlotNames[s] << "：「"
                          << (it != pool.end() ? it->second->getName() : id) << "」\n";
            }
        }

        // 物品清单：装备在上、药水中间、材料最下，编号跨组连续
        static const char* kGroupNames[3] = { "装备", "药水", "材料" };
        std::vector<BagEntry> entries;
        collectBagEntries(player, pool, entries);
        int curGroup = -1;
        int count = 0;
        for (const auto& e : entries) {
            int g = categoryRank(e.category);
            if (g != curGroup) {
                curGroup = g;
                std::cout << "\n【" << kGroupNames[curGroup] << "】\n";
            }
            count++;
            std::string tag;
            if (e.slot >= 0) {
                tag = "（" + std::string(kSlotNames[e.slot]);
                if (player->getEquippedItemId(e.slot) == e.itemId) tag += "·已装配";
                tag += "）";
            }
            std::cout << "  " << count << ". " << e.name << " ×" << e.count << tag << "\n";
        }
        if (count == 0) std::cout << "\n  背包是空的。\n";

        std::cout << "\n指令：equip+编号 装配 / unequip+编号 卸下 / use+编号 使用药水 / 回车或ESC 返回地图\n> ";
        std::string cmd = readCommandLine();
        if (cmd.empty()) return; // 直接回车或 ESC 退出背包

        // 解析指令（equipN / useN，兼容 equip N / use N）
        std::string lower;
        for (char ch : cmd) lower += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        std::string act;
        if (lower.compare(0, 7, "unequip") == 0) act = "unequip";
        else if (lower.compare(0, 5, "equip") == 0) act = "equip";
        else if (lower.compare(0, 3, "use") == 0) act = "use";
        else {
            std::cout << "未知指令：请使用 equip+编号 / unequip+编号 / use+编号。\n";
            console::pause();
            continue;
        }
        std::string digits;
        size_t start = (act == "equip") ? 5 : (act == "unequip" ? 7 : 3);
        for (size_t i = start; i < lower.size(); i++)
            if (lower[i] >= '0' && lower[i] <= '9') digits += lower[i];
        int n = 0;
        for (char ch : digits) n = n * 10 + (ch - '0');
        if (n <= 0 || n > static_cast<int>(entries.size())) {
            std::cout << "编号无效，请输入 1~" << entries.size() << "。\n";
            console::pause();
            continue;
        }

        const BagEntry& e = entries[n - 1];
        auto it = pool.find(e.itemId);
        if (act == "equip") {
            const Equipment* eq = (it != pool.end()) ? dynamic_cast<const Equipment*>(it->second.get()) : nullptr;
            if (eq == nullptr) {
                std::cout << "「" << e.name << "」不是装备，无法装配。\n";
            } else {
                int bonus[4];
                eq->getStatBonus(bonus);
                int slot = static_cast<int>(eq->getSlot());
                player->equipItem(slot, eq->getId(), bonus);
                std::cout << "装配成功：「" << eq->getName() << "」→ " << kSlotNames[slot] << "。\n";
            }
        } else if (act == "unequip") { // 卸下装备
            if (e.slot < 0) {
                std::cout << "「" << e.name << "」不是装备，无法卸下。\n";
            } else if (player->getEquippedItemId(e.slot) != e.itemId) {
                std::cout << "「" << e.name << "」当前未装配。\n";
            } else {
                player->unequipItem(e.slot);
                std::cout << "卸下了「" << e.name << "」：" << kSlotNames[e.slot] << "已空出。\n";
            }
        } else { // use：使用药水，恢复生命/内力
            const Consumable* pot = (it != pool.end()) ? dynamic_cast<const Consumable*>(it->second.get()) : nullptr;
            if (pot == nullptr || (pot->getHealHP() <= 0 && pot->getHealSP() <= 0)) {
                std::cout << "「" << e.name << "」不是可用药水。\n";
            } else if (!player->consumeItem(e.itemId, 1)) {
                std::cout << "「" << e.name << "」数量不足。\n";
            } else {
                int h = pot->getHealHP(), s = pot->getHealSP();
                if (h > 0) player->heal(h);
                if (s > 0) player->restoreSP(s);
                std::cout << "使用了「" << pot->getName() << "」";
                if (h > 0) std::cout << "，恢复 " << h << " 点生命";
                if (s > 0) std::cout << "，恢复 " << s << " 点内力";
                std::cout << "。当前 HP " << player->getHP() << " / SP " << player->getSP() << "。\n";
            }
        }
        console::pause();
    }
}

int main(int argc, char* argv[]) {
    console::init();

    // 加载物品/技能数据与玩家角色（含初始背包）
    GameData gameData = DataLoader::loadGameData("data/");
    auto player = DataLoader::loadCombatant("data/player.json", gameData.skillPool);

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

    grid.onItem = [&](const std::string& name) {
        console::setColor(14);
        std::cout << "\n[拾取] 获得「" << name << "」！" << std::endl;
        console::setColor(7);
        if (name == "木炭")       std::cout << "灰烬中的木炭，突围时可绘制地图。" << std::endl;
        else if (name == "楚酒")  std::cout << "半坛楚酒，饮下提士气，但会微醺。" << std::endl;
        else if (name == "铜镜")  std::cout << "虞姬的半面铜镜，似有故事。" << std::endl;
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

    std::cout << "垓下营地 —— WASD 移动项羽，B 背包，ESC 退出" << std::endl;
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
