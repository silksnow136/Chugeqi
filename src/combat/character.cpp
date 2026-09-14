#include "character.h"
#include "skill.h" // 需要SkillBase定义
#include <algorithm>
#include <cmath>
#include <iostream>

// ---- Combatant ----
Combatant::Combatant(const std::string& name, int level, int hp, int sp, int exp,
                     const int baseStats[3], const std::vector<SkillBase*>& skills,
                     const std::unordered_map<std::string, int>& inventory, const std::string& id,
                     int maxHp, int maxSp)
    : Character(name), id(id), hp(hp), sp(sp), level(level), exp(exp), statusFlags(0), skills(skills), inventory(inventory) {
    std::copy(baseStats, baseStats + 3, this->baseStats);
    std::fill(equipmentBonus, equipmentBonus + 3, 0);
    std::fill(&slotBonuses[0][0], &slotBonuses[0][0] + 6, 0);
    // 最大HP/SP未显式给出时，按当前值回退（模板加载时当前值即满值）
    this->maxHp = (maxHp >= 0) ? maxHp : hp;
    this->maxSp = (maxSp >= 0) ? maxSp : sp;
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
    if (hp > maxHp) hp = maxHp;
}

void Combatant::restoreSP(int amount) {
    sp += amount;
    if (sp > maxSp) sp = maxSp;
    if (sp < 0) sp = 0;
}

void Combatant::addStatusEffect(StatusEffect type, int duration) {
    // 如果已有同类型，刷新持续时间
    auto it = std::find_if(activeStatusEffects.begin(), activeStatusEffects.end(),
        [type](const StatusEffectInstance& effect) { return effect.type == type; });
    if (it != activeStatusEffects.end()) {
        it->duration = duration;
    } else {
        StatusEffectInstance effect{type, duration};
        activeStatusEffects.push_back(effect);
    }
    recalcStatusFlags();
}

void Combatant::updateStatusEffects() {
    for (auto it = activeStatusEffects.begin(); it != activeStatusEffects.end(); ) {
        if (it->type == StatusEffect::Stun) {
            // 眩晕在目标自己的回合被跳过时消费，不按轮次递减（避免敌方后手施加的眩晕
            // 在轮末被清掉，导致对友方单位无效）
            ++it;
            continue;
        }
        it->duration--;
        if (it->duration <= 0) {
            it = activeStatusEffects.erase(it);
        } else {
            ++it;
        }
    }
    recalcStatusFlags();
}

void Combatant::removeStatusEffect(StatusEffect type) {
    activeStatusEffects.erase(
        std::remove_if(activeStatusEffects.begin(), activeStatusEffects.end(),
            [type](const StatusEffectInstance& effect) { return effect.type == type; }),
        activeStatusEffects.end());
    recalcStatusFlags();
}

void Combatant::recalcStatusFlags() {
    statusFlags = 0;
    for (const auto& effect : activeStatusEffects) {
        statusFlags |= static_cast<uint16_t>(effect.type);
    }
}

int Combatant::getEffectiveStat(int index) const {
    if (index < 0 || index >= 3) return 0;
    return baseStats[index] + equipmentBonus[index];
}

int Combatant::getHP() const { return hp; }
int Combatant::getSP() const { return sp; }
int Combatant::getMaxHP() const { return maxHp; }
int Combatant::getMaxSP() const { return maxSp; }
int Combatant::getLevel() const { return level; }
int Combatant::getExp() const { return exp; }

int Combatant::getBaseStat(int index) const {
    if (index < 0 || index >= 3) return 0;
    return baseStats[index];
}

void Combatant::setEquipmentBonus(const int bonus[3]) {
    std::copy(bonus, bonus + 3, equipmentBonus);
}

void Combatant::clearEquipmentBonus() {
    std::fill(equipmentBonus, equipmentBonus + 3, 0);
}

bool Combatant::equipItem(int slot, const std::string& itemId, const int bonus[3]) {
    if (slot < 0 || slot > 1) return false;
    if (!equippedItemIds[slot].empty()) unequipItem(slot); // 同槽已有装备则先卸下
    equippedItemIds[slot] = itemId;
    for (int i = 0; i < 3; i++) {
        slotBonuses[slot][i] = bonus[i];
        equipmentBonus[i] += bonus[i];
    }
    return true;
}

void Combatant::unequipItem(int slot) {
    if (slot < 0 || slot > 1 || equippedItemIds[slot].empty()) return;
    for (int i = 0; i < 3; i++) {
        equipmentBonus[i] -= slotBonuses[slot][i];
        slotBonuses[slot][i] = 0;
    }
    equippedItemIds[slot].clear();
}

const std::string& Combatant::getEquippedItemId(int slot) const {
    static const std::string empty;
    if (slot < 0 || slot > 1) return empty;
    return equippedItemIds[slot];
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
    for (int i = 0; i < 3; ++i) baseStats[i] += 1;
    // 提升上限并恢复满HP/SP
    maxHp += 10;
    maxSp += 5;
    hp = maxHp;
    sp = maxSp;
}

void Combatant::showStats() const {
    std::cout << "\n========== 角色属性 ==========\n";
    std::cout << "姓名: " << getName() << "\n";
    std::cout << "等级: " << level << "\n";
    std::cout << "生命: " << hp << "/" << maxHp << "  技能值: " << sp << "/" << maxSp << "\n";
    std::cout << "经验: " << exp << "/" << (10 * level * level) << "\n";
    std::cout << "力量: " << getEffectiveStat(0)
              << "  耐力: " << getEffectiveStat(1)
              << "  敏捷: " << getEffectiveStat(2) << "\n";
    std::cout << "==============================\n";
}
