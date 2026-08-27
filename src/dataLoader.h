#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "character.h"
#include "skill.h"
#include "item.h"
#include "combatSystem.h"

// ---------------------------------------------------------------------------
// 数据分层说明：
//   1. 静态模板（只读定义，适合 JSON）—— 技能池、物品池、敌人模板等，
//      载入一次长期维护，由 GameData 持有，独立释放。
//   2. 运行时持久状态（频繁读写、需存档，适合 SQL/SQLite）—— 友方角色的
//      当前 HP/SP、等级/经验、持有技能、持有道具等，属于玩家存档。
//   3. 战斗临时数据（内存实时维护）—— Battle 中的战斗者实例，随战斗结束释放。
// ---------------------------------------------------------------------------

// 全局静态数据（JSON 载入，长期维护）
struct GameData {
    SkillPool skillPool;
    ItemPool itemPool; // 道具未实现，保持为空
};

// 一场战斗的运行时数据（敌方模板实例 + 配置，不含我方队伍）
struct Battle {
    std::vector<std::unique_ptr<Combatant>> enemies;
    CombatSystem::CombatConfig config;
};

class DataLoader {
public:
    static std::string readFileText(const std::string& path);
    static SkillPool loadSkills(const std::string& path);
    static ItemPool loadItems(const std::string& path);
    static std::unique_ptr<Combatant> loadCombatant(const std::string& path,
                                                    const SkillPool& skillPool);

    // 载入全局静态数据（技能池、物品池等）
    static GameData loadGameData(const std::string& dataDir);

    // 从 battle_<description>.json 载入我方队伍的初始模板（首次建档用）
    static std::vector<std::unique_ptr<Combatant>> loadPartyTemplates(const std::string& battlePath,
                                                                      const SkillPool& skillPool);
    // 从单个 battle_<description>.json 载入敌方与战斗配置
    static Battle loadBattle(const std::string& battlePath, const GameData& gameData);
};
