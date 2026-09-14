#pragma once
#include <string>
#include <vector>
#include <memory>
#include "combat/character.h"
#include "combat/skill.h"
#include "combat/item.h"
#include "story/QuestState.h"

// ---------------------------------------------------------------------------
// 存档管理（JSON 文件）：维护我方角色的运行时状态 + 游戏进度元信息。
//   静态模板（技能池/物品池/敌人模板）仍由 JSON 管理，这里只存可变的玩家状态。
//
//   支持 2 个存档位（slot = 1 / 2），每槽位一个文件 saves/save_<slot>.json。
//
//   文件结构：
//   {
//     "scene": 2, "branch": 1, "gold": 88,
//     "quest": { morale, q1, q2, q3, q3choice, deserters,
//                deserterTalked[], yinlingUnlocked, guanyingDefeated, raincoatWarned },
//     "party": [ { id, name, level, hp, sp, maxHp, maxSp, exp, str, mag, end, agi,
//                  skills: [...], items: {...}, equip: {...} } ]
//   }
// ---------------------------------------------------------------------------
class SaveManager {
public:
    static constexpr int SLOT_COUNT = 2;

    // 存档元信息：剧情进度 / 第二幕分支 / 金币 / 支线进度
    struct Meta {
        int sceneId;
        int branchId;
        int gold;
        QuestState quest;
        Meta() : sceneId(0), branchId(0), gold(0) {}
    };

    // 存档位摘要（供存读档界面展示，无需加载完整队伍）
    struct SlotInfo {
        bool hasSave = false;
        int level = 0;
        std::string name;
        int gold = 0;
        int sceneId = 0;
        int branchId = 0;
    };

    // 一次读档的完整结果（队伍 + 元信息）
    struct SaveData {
        std::vector<std::unique_ptr<Combatant>> party;
        Meta meta;
    };

    explicit SaveManager(const std::string& saveDir = "saves");
    ~SaveManager() = default;

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    static bool validSlot(int slot) { return slot >= 1 && slot <= SLOT_COUNT; }

    bool hasSave(int slot) const;
    SlotInfo getSlotInfo(int slot) const;

    // 保存整槽位（队伍 + 元信息）；只读取队伍，不接管所有权
    void save(int slot, const std::vector<Combatant*>& party,
              const SkillPool& skillPool, const Meta& meta = Meta{}) const;
    void save(int slot, const std::vector<std::unique_ptr<Combatant>>& party,
              const SkillPool& skillPool, const Meta& meta = Meta{}) const;

    // 读取整槽位；空槽位返回 party 为空、meta 为默认值
    SaveData load(int slot, const SkillPool& skillPool, const ItemPool& itemPool) const;

    void resetSave(int slot) const;

private:
    std::string dir_;
    std::string path(int slot) const;
};
