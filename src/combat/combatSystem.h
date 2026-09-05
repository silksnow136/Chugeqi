#pragma once
// 战斗系统：负责回合流程、行动执行与界面显示。
// 以下功能仅保留接口与定义，未实现
//   - 道具使用
//   - 状态效果（灼烧 / 迟缓 / 眩晕 / 充能）
//   - 充能技能的实际执行
#include <vector>
#include <memory>
#include <deque>
#include <string>
#include <unordered_map>
#include <random>
#include "character.h"
#include "skill.h"
#include "item.h"
#include "battleLog.h"

class CombatSystem {
public:
    // 战斗配置
    struct CombatConfig {
        bool disableRun = false;   // 是否禁止逃跑
        bool disableItems = false; // 是否禁止使用物品（道具系统未实现）
        bool playerFirst = true;   // 是否我方先手（当前未使用）
    };

    // 行动结果（预留，当前未使用）
    struct ActionResult {
        std::string description;
        int damage = 0;
        bool hit = false;
        bool critical = false;
    };

    CombatSystem(Combatant* player, std::vector<Combatant*> companions,
                 std::vector<Combatant*> enemies,
                 const CombatConfig& config);
    ~CombatSystem();

    // 启动战斗，返回是否胜利
    bool startBattle();

    // 获取战斗日志
    const std::deque<std::string>& getLog() const;

private:
    // 参战角色
    Combatant* player;
    std::vector<Combatant*> companions;
    std::vector<Combatant*> enemies;
    CombatConfig config;

    // 战斗状态
    bool battleEnded = false;
    bool playerWon = false;
    bool playerAiAssisted = false; // 玩家是否开启全员 AI 托管
    bool companionAiAssisted = false; // 同伴独立 AI 托管（同伴菜单切换）
    BattleLog log;
    mutable std::mt19937 rng; // 随机数引擎，构造时用 random_device 播种

    // 随机数辅助：[0, upper) 均匀分布
    int roll(int upper) const;

    // 界面
    void addLog(const std::string& msg);
    void displayBattle() const;
    void displayStatus(const Combatant* c) const;

    // 回合处理
    bool processPlayerTurn();
    bool processCompanionTurn(Combatant* companion);
    bool processEnemyTurn(Combatant* enemy);
    bool processAllyAITurn(Combatant* actor); // 我方 AI 托管回合（玩家可切换开启）
    bool manualTurn(Combatant* actor, int maxChoice); // 玩家/同伴共用的手动回合
    void aiPause(); // AI 行动后的等待：任意键继续，ESC 退出玩家 AI 托管

    // 行动执行
    bool performAttack(Combatant* attacker, Combatant* target, bool isNormalAttack = true);
    bool performSkill(Combatant* user, SkillBase* skill, std::vector<Combatant*>& targets);
    bool performItem(Combatant* user, const std::string& itemId, std::vector<Combatant*>& targets); // 未实现
    bool attemptRun(Combatant* runner);

    // 辅助计算
    float calculateHitRate(float baseHitRate, int attackerAgility, int defenderAgility);
    int calculateDamage(int strength, int power, int defense);
    std::vector<Combatant*> getAliveEnemies() const;
    std::vector<Combatant*> getAliveAllies() const;

    // 状态效果结算
    void applyStatusEffects(Combatant* c); // 回合开始：对单个战斗者结算（灼烧扣血 / 迟缓提示）
    void applyRoundStartStatus();          // 回合开始：对全体存活战斗者统一结算
    void applyRoundEndStatus();            // 回合结束：对全体存活战斗者递减状态持续回合

    // AI 决策
    SkillBase* chooseAISkill(Combatant* ai, const std::vector<Combatant*>& enemies, const std::vector<Combatant*>& allies);
    Combatant* chooseAITarget(Combatant* ai, const std::vector<Combatant*>& potentialTargets);
};
