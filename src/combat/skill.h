#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "character.h"

enum class AttackScope {
    Single,
    All
};

// ---------------------------------------------------------------------------
// 技能体系：SkillBase 为基类，派生 DamageSkill（伤害）、HealSkill（治疗）、
// ChargingSkill（充能/属性提升）。技能对象由 DataLoader 从 skill.json 创建并
// 长期持有；实际执行逻辑统一放在 CombatSystem 中按 dynamic_cast 分派。
// ---------------------------------------------------------------------------

// 技能基类
class SkillBase {
private:
    std::string name;
    std::string description;
    int cost;        // SP消耗（对于恢复SP的技能，可能为负表示消耗HP，需另行处理）
    AttackScope scope;
public:
    SkillBase(const std::string& name, const std::string& description, int cost, AttackScope scope);
    virtual ~SkillBase() = default;

    const std::string& getName() const;
    const std::string& getDescription() const;
    int getCost() const;
    AttackScope getScope() const;

    // 可重载执行函数，但实际执行放在CombatSystem中
};

// 伤害技能
class DamageSkill : public SkillBase {
private:
    int power;
    StatusEffect statusEffect;
    float hitRate; // 基础命中率
public:
    DamageSkill(const std::string& name, const std::string& description, int cost, int power,
                AttackScope scope, StatusEffect statusEffect = StatusEffect::None, float hitRate = 0.95f);
    int getPower() const;
    StatusEffect getStatusEffect() const;
    float getHitRate() const;
    int calculateDamage(int attackerStrength, int defenderDefense) const; 
};

// 治疗技能
class HealSkill : public SkillBase {
private:
    int healAmount;
public:
    HealSkill(const std::string& name, const std::string& description, int cost, int healAmount, AttackScope scope);
    int getHealAmount() const;
};

// 充能技能（提升属性）
class ChargingSkill : public SkillBase {
private:
    float multiplier;    // 属性提升倍率
    int targetStat; // -1全属性，0~3具体属性
    int duration;   // 持续回合数
public:
    ChargingSkill(const std::string& name, const std::string& description, int cost,
                  float multiplier, int targetStat, int duration, AttackScope scope);
    float getMultiplier() const;
    int getTargetStat() const;
    int getDuration() const;
};

// 技能池：由 DataLoader 长期持有，唯一拥有 SkillBase 对象的所有权。
using SkillPool = std::unordered_map<std::string, std::unique_ptr<SkillBase>>;