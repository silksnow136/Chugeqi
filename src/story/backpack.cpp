#include "backpack.h"
#include "core/console.h"
#include <iostream>
#include <algorithm>
#include <cctype>

// 槽位名称（与 EquipmentSlot 一致：0=防具 1=武器 2=鞋子 3=配饰）
static const char* kSlotNames[4] = { "防具", "武器", "鞋子", "配饰" };

// 分类排序权重：装备0 药水1 材料2
static int categoryRank(const std::string& cat) {
    if (cat == "equipment") return 0;
    if (cat == "potion") return 1;
    return 2;
}

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

        std::cout << "\n指令：equip+编号 装配 / unequip+编号 卸下 / use+编号 使用药水 / show 查看属性 / 回车或ESC 返回地图\n> ";
        std::string cmd = console::readLine();
        if (cmd.empty()) return; // 直接回车或 ESC 退出背包

        // 解析指令（equipN / unequipN / useN / show，兼容带空格的写法）
        std::string lower;
        for (char ch : cmd) lower += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        std::string act;
        if (lower.compare(0, 7, "unequip") == 0) act = "unequip";
        else if (lower.compare(0, 5, "equip") == 0) act = "equip";
        else if (lower.compare(0, 3, "use") == 0) act = "use";
        else if (lower.compare(0, 4, "show") == 0) act = "show";
        else {
            std::cout << "未知指令：请使用 equip+编号 / unequip+编号 / use+编号 / show。\n";
            console::pause();
            continue;
        }
        if (act == "show") { // 查看角色属性，无需编号
            player->showStats();
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
