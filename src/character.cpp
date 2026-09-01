#include "character.h"
#include "skill.h" // 需要SkillBase定义
#include <algorithm>
#include <cmath>

// ---- Civil ----
Civil::Civil(const std::string& name, const std::string& role, const std::vector<std::string>& dialogue)
    : Character(name), role(role), dialogue(dialogue) {}

const std::string& Civil::getRole() const { return role; }
const std::string& Civil::getDialogue(int index) const { return dialogue.at(index); }

// ---- Combatant ----
Combatant::Combatant(const std::string& name, int level, int hp, int sp, int exp,
                     const int baseStats[4], const std::vector<SkillBase*>& skills,
                     const std::unordered_map<std::string, int>& inventory, const std::string& id)
    : Character(name), id(id), hp(hp), sp(sp), level(level), exp(exp), statusFlags(0), skills(skills), inventory(inventory) {
    std::copy(baseStats, baseStats + 4, this->baseStats);
    std::fill(equipmentBonus, equipmentBonus + 4, 0);
}

const std::string& Combatant::getId() const { return id; }

Combatant::~Combatant() {
    // 注意：skills中的SkillBase指针由外部管理（如SkillManager），这里不删除
}

bool Combatant::hasStatusEffect(StatusEffect effect) const {
    return (statusFlags & static_cast<uint16_t>(effect)) != 0;
}

bool Combatant::isAlive() const { return hp > 0; }

void Combatant::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

void Combatant::heal(int amount) {
    hp += amount;
    // 可考虑设上限，但暂不设
}

void Combatant::restoreSP(int amount) {
    sp += amount;
}

void Combatant::addStatusEffect(StatusEffect type, int duration, int targetStat, float mult) {
    // 如果已有同类型，刷新持续时间
    auto it = std::find_if(activeStatusEffects.begin(), activeStatusEffects.end(),
        [type](const StatusEffectInstance& inst) { return inst.type == type; });
    if (it != activeStatusEffects.end()) {
        it->duration = duration;
        if (type == StatusEffect::Charge) {
            it->targetStatIndex = targetStat;
            it->multiplier = mult;
        }
    } else {
        StatusEffectInstance inst{type, duration, targetStat, mult};
        activeStatusEffects.push_back(inst);
    }
    recalcStatusFlags();
}

void Combatant::updateStatusEffects() {
    for (auto it = activeStatusEffects.begin(); it != activeStatusEffects.end(); ) {
        it->duration--;
        if (it->duration <= 0) {
            it = activeStatusEffects.erase(it);
        } else {
            ++it;
        }
    }
    recalcStatusFlags();
}

void Combatant::clearStatusEffect(StatusEffect type) {
    activeStatusEffects.erase(
        std::remove_if(activeStatusEffects.begin(), activeStatusEffects.end(),
            [type](const StatusEffectInstance& inst) { return inst.type == type; }),
        activeStatusEffects.end());
    recalcStatusFlags();
}

void Combatant::recalcStatusFlags() {
    statusFlags = 0;
    for (const auto& inst : activeStatusEffects) {
        statusFlags |= static_cast<uint16_t>(inst.type);
    }
}

int Combatant::getEffectiveStat(int index) const {
    if (index < 0 || index >= 4) return 0;
    int stat = baseStats[index] + equipmentBonus[index];
    // 状态效果修正：迟缓降低敏捷，充能按倍率提升指定属性
    for (const auto& inst : activeStatusEffects) {
        if (inst.type == StatusEffect::Slow && index == 3) {
            stat = stat * 3 / 5; // 迟缓：敏捷降为原来的 60%
        } else if (inst.type == StatusEffect::Charge &&
                   (inst.targetStatIndex == -1 || inst.targetStatIndex == index)) {
            stat = static_cast<int>(stat * inst.multiplier);
        }
    }
    return stat;
}

int Combatant::getHP() const { return hp; }
int Combatant::getSP() const { return sp; }
int Combatant::getLevel() const { return level; }
int Combatant::getExp() const { return exp; }

void Combatant::setEquipmentBonus(const int bonus[4]) {
    std::copy(bonus, bonus + 4, equipmentBonus);
}

void Combatant::clearEquipmentBonus() {
    std::fill(equipmentBonus, equipmentBonus + 4, 0);
}

void Combatant::addSkill(SkillBase* skill) {
    skills.push_back(skill);
}

const std::vector<SkillBase*>& Combatant::getSkills() const {
    return skills;
}

bool Combatant::hasItem(const std::string& itemId) const {
    auto it = inventory.find(itemId);
    return it != inventory.end() && it->second > 0;
}

int Combatant::getItemCount(const std::string& itemId) const {
    auto it = inventory.find(itemId);
    return (it != inventory.end()) ? it->second : 0;
}

const std::unordered_map<std::string, int>& Combatant::getInventory() const {
    return inventory;
}

void Combatant::addItem(const std::string& itemId, int count) {
    inventory[itemId] = std::min(99, inventory[itemId] + count);
}

bool Combatant::consumeItem(const std::string& itemId, int count) {
    auto it = inventory.find(itemId);
    if (it == inventory.end() || it->second < count) return false;
    it->second -= count;
    if (it->second == 0) inventory.erase(it);
    return true;
}

void Combatant::addExp(int amount) {
    exp += amount;
    int needed = static_cast<int>(10 * level * level); // base=10, exp_next = base*level^2
    while (exp >= needed && level < 99) {
        exp -= needed;
        levelUp();
        needed = static_cast<int>(10 * level * level);
    }
}

void Combatant::levelUp() {
    level++;
    // 全属性+1
    for (int i = 0; i < 4; ++i) baseStats[i] += 1;
    // 恢复满HP和SP
    hp = 100 + level * 10;
    sp = 50 + level * 5;
}