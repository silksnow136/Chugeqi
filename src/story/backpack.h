#pragma once
// ---------------------------------------------------------------------------
// 背包 / 物品通用工具（装配、卸下、使用、属性查看）
// ---------------------------------------------------------------------------
#include <string>
#include <vector>
#include "combat/character.h"
#include "combat/item.h"

// 背包条目（分组显示用）
struct BagEntry {
    std::string itemId;
    std::string name;
    int count;
    std::string category; // equipment 装备 / potion 药水 / material 材料
    int slot;             // 装备槽位，非装备为 -1
};

// 收集背包条目并按 装备->药水->材料 排序
void collectBagEntries(Combatant* player, const ItemPool& pool, std::vector<BagEntry>& out);

// 背包界面：显示当前装备与分类物品清单；
// equip+编号 装配 / unequip+编号 卸下 / use+编号 使用药水 / show 查看属性；回车或ESC返回
void showBackpack(Combatant* player, const ItemPool& pool);
