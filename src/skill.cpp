#include "skill.h"

// ---- SkillBase ----
SkillBase::SkillBase(const std::string& name, const std::string& description, int cost, AttackScope scope)
    : name(name), description(description), cost(cost), scope(scope) {}

const std::string& SkillBase::getName() const { return name; }
const std::string& SkillBase::getDescription() const { return description; }
int SkillBase::getCost() const { return cost; }
AttackScope SkillBase::getScope() const { return scope; }

// ---- DamageSkill ----
DamageSkill::DamageSkill(const std::string& name, const std::string& description, int cost, int power,
                         AttackScope scope, StatusEffect statusEffect, float hitRate)
    : SkillBase(name, description, cost, scope), power(power), statusEffect(statusEffect), hitRate(hitRate) {}

int DamageSkill::getPower() const { return power; }
StatusEffect DamageSkill::getStatusEffect() const { return statusEffect; }
float DamageSkill::getHitRate() const { return hitRate; }

int DamageSkill::calculateDamage(int attackerStrength, int defenderDefense) const {
    int damage = attackerStrength * power - defenderDefense;
    if (damage < 0) damage = 0;
    return damage;
}

// ---- HealSkill ----
HealSkill::HealSkill(const std::string& name, const std::string& description, int cost, int healAmount, AttackScope scope)
    : SkillBase(name, description, cost, scope), healAmount(healAmount) {}

int HealSkill::getHealAmount() const { return healAmount; }

// ---- ChargingSkill ----
ChargingSkill::ChargingSkill(const std::string& name, const std::string& description, int cost,
                             float multiplier, int targetStat, int duration, AttackScope scope)
    : SkillBase(name, description, cost, scope), multiplier(multiplier), targetStat(targetStat), duration(duration) {}

float ChargingSkill::getMultiplier() const { return multiplier; }
int ChargingSkill::getTargetStat() const { return targetStat; }
int ChargingSkill::getDuration() const { return duration; }