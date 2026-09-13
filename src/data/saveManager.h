#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "combat/character.h"
#include "combat/skill.h"
#include "combat/item.h"

struct sqlite3; // 前向声明，避免头文件暴露 SQLite 细节

// ---------------------------------------------------------------------------
// 存档管理（SQLite）：维护我方角色的运行时状态 + 游戏进度元信息。
//   静态模板（技能池/物品池/敌人模板）仍由 JSON 管理，这里只存可变的玩家状态。
//
//   支持 2 个存档位（slot = 1 / 2）。
//
//   表结构：
//     meta(slot_id, scene_id, branch_id, gold)
//     characters(slot_id, id, name, level, hp, sp, exp, str, mag, endur, agi)
//     learned_skills(slot_id, character_id, slot, skill_id)
//     inventory(slot_id, character_id, item_id, count)
//     equipment(slot_id, character_id, equip_slot, item_id)
// ---------------------------------------------------------------------------
class SaveManager {
public:
    static constexpr int SLOT_COUNT = 2;

    // 存档元信息（每存档位一份）：剧情进度 / 第二幕分支 / 金币
    struct Meta {
        int sceneId = 0;
        int branchId = 0;
        int gold = 0;
    };

    explicit SaveManager(const std::string& dbPath);
    ~SaveManager();

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // 校验存档位合法性
    static bool validSlot(int slot) { return slot >= 1 && slot <= SLOT_COUNT; }

    // 该存档位是否已有存档（characters 表非空）
    bool hasSave(int slot) const;

    // 队伍存档/读档（技能指针由 skillPool 解析，装备由 itemPool 解析）
    // saveParty 只读取队伍状态，不接管所有权；提供两种指针形态便于调用。
    void saveParty(int slot, const std::vector<Combatant*>& party,
                   const SkillPool& skillPool) const;
    void saveParty(int slot, const std::vector<std::unique_ptr<Combatant>>& party,
                   const SkillPool& skillPool) const;
    std::vector<std::unique_ptr<Combatant>> loadParty(int slot,
                                                       const SkillPool& skillPool,
                                                       const ItemPool& itemPool) const;

    // 元信息存档/读档
    void saveMeta(int slot, const Meta& meta) const;
    Meta loadMeta(int slot) const;

    // 清空该存档位（删除全部相关记录）
    void resetSave(int slot) const;

private:
    sqlite3* db = nullptr;
};
