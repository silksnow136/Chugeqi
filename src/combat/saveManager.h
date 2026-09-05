#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "character.h"
#include "skill.h"

struct sqlite3; // 前向声明，避免头文件暴露 SQLite 细节

// ---------------------------------------------------------------------------
// 存档管理（SQLite）：
//   维护我方角色的“运行时状态”——当前 HP/SP、等级/经验、属性、持有技能、持有道具。
//   静态模板（技能池/物品池/敌人模板）仍由 JSON 管理，这里只存可变的玩家状态。
//
//   表结构：
//     characters(id, name, level, hp, sp, exp, str, mag, endur, agi)
//     learned_skills(character_id, slot, skill_id)
//     inventory(character_id, item_id, count)
// ---------------------------------------------------------------------------
class SaveManager {
public:
    explicit SaveManager(const std::string& dbPath);
    ~SaveManager();

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // 是否已有存档（characters 表非空）
    bool hasSave() const;

    // 从存档加载我方队伍（技能指针由 skillPool 解析）
    std::vector<std::unique_ptr<Combatant>> loadParty(const SkillPool& skillPool) const;

    // 把我方队伍的当前状态写入存档（技能指针反向映射为 skill_id）
    void saveParty(const std::vector<std::unique_ptr<Combatant>>& party,
                   const SkillPool& skillPool) const;

    // 清空存档（删除 characters / learned_skills / inventory 全部记录）
    void resetSave() const;

private:
    sqlite3* db = nullptr;
};
