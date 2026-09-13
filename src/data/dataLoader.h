#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "combat/character.h"
#include "combat/skill.h"
#include "combat/item.h"
#include "combat/combatSystem.h"

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

// ---------------------------------------------------------------------------
// 剧情叙事数据（story.json 载入）
// ---------------------------------------------------------------------------

// 一行叙事（旁白或对话）
struct StoryLine {
    std::string text;      // 显示文本（可含 \n）
    int color = 14;        // 文字颜色（Windows 控制台色码）
    int sleep = 0;         // 播完后停顿毫秒
    bool wait = false;     // 是否等待按键后继续（对应 nextLine）
};

// 分支选项
struct StoryOption {
    char key = 'a';                // 触发键
    std::string text;              // 选项文本
    int branch = 0;                // 分支 id
    std::vector<StoryLine> lines;  // 选中后的叙事
};

// 分支选择
struct StoryChoice {
    std::string prompt;             // 选择提示文本
    std::vector<StoryOption> options;
};

// 一幕场景
struct Scene {
    int id = 0;
    std::vector<StoryLine> lines;  // 顺序播放的叙事
    StoryChoice choice;            // 可选分支（options 为空表示无分支）
};

// 整个剧情
struct StoryData {
    std::vector<Scene> scenes;
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

    // 从 story.json 载入剧情（解析失败或文件缺失时抛 std::runtime_error）
    static StoryData loadStory(const std::string& path);
};
