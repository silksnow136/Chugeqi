#include <string>
#include <vector>

class character {
private:
    std::string name;
public:
    character(const std::string& name) : name(name) {}
    const std::string& getName() const { return name; }
};

class Civil : public character {  // 非战斗角色，有对话和职能标签
private:
    std::string role;
    std::vector<std::string> dialogue;
public:
    Civil(const std::string& name, const std::string& role, const std::vector<std::string>& dialogue)
        : character(name), role(role), dialogue(dialogue) {}
    const std::string& getRole() const { return role; }
    const std::string& getDialogue(int index) const { return dialogue.at(index); }
};

enum class StatusEffect : uint16_t {
    None   = 0,
    Burn   = 1 << 0,  // 灼烧，每回合扣除一定生命值
    Slow   = 1 << 1,  // 迟缓，降低agility（敏捷）
    Stun   = 1 << 2,  // 眩晕，使目标无法行动
};

enum class AttackScope {    // 攻击范围，个体或全体
    Single,
    All
};

class SkillBase {
private:
    std::string name;
    std::string description;
    AttackScope scope;
    int cost;
public:
    SkillBase(const std::string& name, const std::string& description, int cost, AttackScope scope)
        : name(name), description(description), cost(cost), scope(scope) {}
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    int getCost() const { return cost; }
    AttackScope getScope() const { return scope; }
};

class DamageSkill : public SkillBase {       // 攻击技能
private:
    int power;
    StatusEffect statusEffect = StatusEffect::None;  // 攻击附加状态，默认无
    float HitRate = 1.0f;  // 命中率，默认100%
public:
    DamageSkill(const std::string& name, const std::string& description, int cost, int power, AttackScope scope, StatusEffect statusEffect = StatusEffect::None, float hitRate = 1.0f)
        : SkillBase(name, description, cost, scope), power(power), statusEffect(statusEffect), HitRate(hitRate) {}
    int getPower() const { return power; }
    StatusEffect getStatusEffect() const { return statusEffect; }
    int calculateDamage(int attackerStrength, int defenderDefense) const {  // 计算伤害
        return (attackerStrength * power) - defenderDefense;
    }
    
};

class HealSkill : public SkillBase {         // 治疗技能
private:
    int healAmount;
public:
    HealSkill(const std::string& name, const std::string& description, int cost, int healAmount, AttackScope scope)
        : SkillBase(name, description, cost, scope), healAmount(healAmount) {}
    int getHealAmount() const { return healAmount; }
};

class CharingSkill : public SkillBase {       // 充能技能，如蓄力
private:
    float chargeMutiplier;  // 充能倍率
public:
    CharingSkill(const std::string& name, const std::string& description, int cost, float chargeMutiplier, AttackScope scope)
        : SkillBase(name, description, cost, scope), chargeMutiplier(chargeMutiplier) {}
    float getChargeMutiplier() const { return chargeMutiplier; }
};

class Combatant : public character {
private:
    int hp;
    int sp;
    int level;
    int exp;
    int stats[5]; // strength, magic, endurance, agility, luck
    uint16_t statusFlags = 0;   // 添加状态：statusFlags |= (uint16_t)StatusEffect::Burn;  检查状态：if (statusFlags & (uint16_t)StatusEffect::Poison) { ... }
    std::vector<SkillBase> skills;
public:
    Combatant(const std::string& name, int level, int hp, int sp, int exp, const int stats[5], const std::vector<SkillBase>& skills)
        : character(name), level(level), hp(hp), sp(sp), exp(exp), skills(skills) {
        std::copy(stats, stats + 5, this->stats);
    }
    bool hasStatusEffect(StatusEffect effect) const { return (statusFlags & (uint16_t)effect) != 0; }
    bool isAlive() const { return hp > 0; }
    void takeDamage(int damage) { hp -= damage; if (hp < 0) hp = 0; }
    void heal(int amount) { hp += amount; }
    void setStatusEffect(StatusEffect effect) { statusFlags |= (uint16_t)effect; }
    void setStats(const int newStats[5]) { std::copy(newStats, newStats + 5, stats); }
    int getStat(int index) const { return stats[index]; }
};