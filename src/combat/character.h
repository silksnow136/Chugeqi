#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

class SkillBase; // 前向声明，避免与 skill.h 循环包含

class Character {
private:
    std::string name;
public:
    Character(const std::string& name) : name(name) {}
    virtual ~Character() = default;
    const std::string& getName() const { return name; }
};

// 状态效果枚举（位标志）
enum class StatusEffect : uint16_t {
    None   = 0,
    Burn   = 1 << 0,   // 灼烧
    Stun   = 1 << 1,   // 眩晕
};

// 状态实例（用于存储持续回合数）
struct StatusEffectInstance {
    StatusEffect type;
    int duration;          // 剩余持续回合数
};

// 战斗角色
class Combatant : public Character {
private:
    std::string id;        // 稳定标识，用于存档/数据库主键（敌人可为空）
    int hp;
    int sp;
    int maxHp;             // 最大生命值
    int maxSp;             // 最大技能值
    int level;
    int exp;
    int baseStats[3];      // strength, endurance, agility
    uint16_t statusFlags;  // 快速检查位
    std::vector<StatusEffectInstance> activeStatusEffects; // 状态详细信息
    std::vector<SkillBase*> skills;
    std::unordered_map<std::string, int> inventory; // 物品ID -> 数量
public:
    Combatant(const std::string& name, int level, int hp, int sp, int exp,
              const int baseStats[3], const std::vector<SkillBase*>& skills,
              const std::unordered_map<std::string, int>& inventory = {},
              const std::string& id = "",
              int maxHp = -1, int maxSp = -1);
    ~Combatant();

    // 稳定标识
    const std::string& getId() const;

    // 状态检查
    bool hasStatusEffect(StatusEffect effect) const;
    bool isAlive() const;
    void takeDamage(int damage);
    void heal(int amount);
    void restoreSP(int amount);

    // 状态管理
    void addStatusEffect(StatusEffect type, int duration);
    void updateStatusEffects(); // 每回合结束调用，减少持续回合，移除到期状态
    void removeStatusEffect(StatusEffect type); // 立即移除指定状态（眩晕在目标回合被跳过时消费）

    // 属性获取（含装备加成）
    int getEffectiveStat(int index) const; // index 0~2：力量/耐力/敏捷
    int getBaseStat(int index) const;      // 基础属性（不含装备加成，供存档）
    int getHP() const;
    int getSP() const;
    int getMaxHP() const;
    int getMaxSP() const;
    int getLevel() const;
    int getExp() const;

    // 装备加成（外部设置）
    void setEquipmentBonus(const int bonus[3]); // 三项属性加成
    void clearEquipmentBonus();

    // 装备槽位管理（槽位与 EquipmentSlot 一致：0=防具 1=武器）
    bool equipItem(int slot, const std::string& itemId, const int bonus[3]); // 装配装备（同槽已有则先卸下）
    void unequipItem(int slot);                                              // 卸下指定槽位装备
    const std::string& getEquippedItemId(int slot) const;                    // 当前装备物品ID，空串=未装备

    // 技能管理
    void addSkill(SkillBase* skill);
    const std::vector<SkillBase*>& getSkills() const;

    // 物品管理
    bool hasItem(const std::string& itemId) const;
    int getItemCount(const std::string& itemId) const;
    const std::unordered_map<std::string, int>& getInventory() const;
    void addItem(const std::string& itemId, int count = 1);
    bool consumeItem(const std::string& itemId, int count = 1);

    // 经验与升级
    void addExp(int amount);
    void levelUp(); // 升级时全属性+1，并恢复满HP/SP

    // 查看基础属性
    void showStats() const;

private:
    int equipmentBonus[3];          // 三项属性加成（各槽位加成之和）
    std::string equippedItemIds[2]; // 每槽位装备的物品ID（空串=未装备）
    int slotBonuses[2][3];          // 每槽位对三项属性的加成
    void recalcStatusFlags(); // 根据activeStatusEffects更新statusFlags
};