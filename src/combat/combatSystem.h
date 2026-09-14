#pragma once
// 战斗系统：负责回合流程、行动执行与界面显示。
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
        bool disableItems = false; // 是否禁止使用物品
    };

    CombatSystem(Combatant* player, std::vector<Combatant*> companions,
                 std::vector<Combatant*> enemies,
                 const CombatConfig& config,
                 const ItemPool* itemPool = nullptr);
    ~CombatSystem();

    // 启动战斗，返回是否胜利（逃跑返回 false，可通过 escaped() 区分）
    bool startBattle();

    // 战斗是否以「成功逃跑」结束
    bool escaped() const { return playerEscaped; }

    // 获取战斗日志
    const std::deque<std::string>& getLog() const;

private:
    // 参战角色
    Combatant* player;
    std::vector<Combatant*> companions;
    std::vector<Combatant*> enemies;
    CombatConfig config;
    const ItemPool* itemPool = nullptr; // 物品池（战斗中解析药品效果），可为空

    // 战斗状态
    bool battleEnded = false;
    bool playerWon = false;
    bool playerEscaped = false; // 是否成功逃跑（与 playerWon 互斥）
    bool playerAiAssisted = false; // 玩家是否开启全员 AI 托管
    bool companionAiAssisted = false; // 同伴独立 AI 托管（同伴菜单切换）
    BattleLog log;
    mutable std::mt19937 rng; // 随机数引擎，构造时用 random_device 播种

    // 随机数辅助：[0, upper) 均匀分布
    int roll(int upper) const;

    // 界面
    void addLog(const std::string& msg);
    void displayBattle();
    std::string displayStatus(const Combatant* c) const; // 返回单行状态文本

    // 回合处理
    void processPlayerTurn();
    void processCompanionTurn(Combatant* companion);
    void processEnemyTurn(Combatant* enemy);
    void processAllyAITurn(Combatant* actor); // 我方 AI 托管回合（玩家可切换开启）
    void manualTurn(Combatant* actor, int maxChoice); // 玩家/同伴共用的手动回合
    void aiPause(); // AI 行动后的等待：任意键继续，ESC 退出玩家 AI 托管

    // 行动执行
    void performAttack(Combatant* attacker, Combatant* target);
    void performSkill(Combatant* user, SkillBase* skill, std::vector<Combatant*>& targets);
    void performItem(Combatant* user, const std::string& itemId);
    bool useItemInBattle(Combatant* actor); // 战斗中道具菜单：use+编号 使用药品
    void attemptRun(Combatant* runner);

    // 辅助计算
    static float calculateHitRate(float baseHitRate, int attackerAgility, int defenderAgility);
    static int calculateDamage(int strength, int power, int defense);
    std::vector<Combatant*> getAliveEnemies() const;
    std::vector<Combatant*> getAliveAllies() const;

    // 状态效果结算
    void applyStatusEffects(Combatant* c); // 回合开始：对单个战斗者结算（灼烧扣血 / 迟缓提示）
    void applyRoundStartStatus();          // 回合开始：对全体存活战斗者统一结算
    void applyRoundEndStatus();            // 回合结束：对全体存活战斗者递减状态持续回合

    // AI 决策
    SkillBase* chooseAISkill(Combatant* ai);
    Combatant* chooseAITarget(Combatant* ai, const std::vector<Combatant*>& potentialTargets);

    // ===== 菜单 / 目标选择 / 状态显示辅助（私有静态成员，便于按职责拆分到不同 .cpp）=====
    static int readMenuChoice(int min, int max, bool allowBack = false);
    static SkillBase* selectSkill(Combatant* actor);
    static Combatant* selectTarget(const std::vector<Combatant*>& candidates, const std::string& prompt);
    static std::vector<Combatant*> buildSkillTargets(const std::vector<Combatant*>& allies,
                                                     const std::vector<Combatant*>& enemies,
                                                     SkillBase* skill);
    static const char* statusName(StatusEffect e);
    static int defaultStatusDuration(StatusEffect e);
};
