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

// 非战斗角色（仅剧情对话，不参与战斗）
class Civil : public Character {
private:
    std::string role;
    std::vector<std::string> dialogue;
public:
    Civil(const std::string& name, const std::string& role, const std::vector<std::string>& dialogue);
    const std::string& getRole() const;
    const std::string& getDialogue(int index) const;
};

// 状态效果枚举（位标志）
enum class StatusEffect : uint16_t {
    None   = 0,
    Burn   = 1 << 0,   // 灼烧
    Slow   = 1 << 1,   // 迟缓
    Stun   = 1 << 2,   // 眩晕
    Charge = 1 << 3,   // 充能（特殊状态，需携带额外数据）
};

// 状态实例（用于存储持续回合和数值）
struct StatusEffectInstance {
    StatusEffect type;
    int duration;          // 剩余持续回合数
    int targetStatIndex;   // -1 表示全属性，0~3 分别对应力量/魔力/耐力/敏捷
    float multiplier;      // 仅对充能有效
};

// 战斗角色
class Combatant : public Character {
private:
    std::string id;        // 稳定标识，用于存档/数据库主键（敌人可为空）
    int hp;
    int sp;
    int level;
    int exp;
    int baseStats[4];      // strength, magic, endurance, agility 
    uint16_t statusFlags;  // 快速检查位
    std::vector<StatusEffectInstance> activeStatusEffects; // 状态详细信息
    std::vector<SkillBase*> skills;
    std::unordered_map<std::string, int> inventory; // 物品ID -> 数量
public:
    Combatant(const std::string& name, int level, int hp, int sp, int exp,
              const int baseStats[4], const std::vector<SkillBase*>& skills,
              const std::unordered_map<std::string, int>& inventory = {},
              const std::string& id = "");
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
    void addStatusEffect(StatusEffect type, int duration, int targetStat = -1, float mult = 0.0f);
    void updateStatusEffects(); // 每回合结束调用，减少持续回合，移除到期状态
    void clearStatusEffect(StatusEffect type);

    // 属性获取（含装备加成）
    int getEffectiveStat(int index) const; // index 0~3
    int getHP() const;
    int getSP() const;
    int getLevel() const;
    int getExp() const;

    // 装备加成（外部设置）
    void setEquipmentBonus(const int bonus[4]); // 四项属性加成
    void clearEquipmentBonus();

    // 技能管理
    void addSkill(SkillBase* skill);
    const std::vector<SkillBase*>& getSkills() const;

    // 物品管理
    bool hasItem(const std::string& itemId) const;
    int getItemCount(const std::string& itemId) const;
    const std::unordered_map<std::string, int>& getInventory() const;
    void addItem(const std::string& itemId, int count = 1);
    bool consumeItem(const std::string& itemId, int count = 1);

    // 拾取与查看（探索态交互）
    void pickUp(const std::string& itemId, int count = 1); // 拾取物品并输出反馈
    void showStats() const;                                // 查看基础属性
    void showInventory() const;                            // 查看背包（itemId×数量）

    // 经验与升级
    void addExp(int amount);
    void levelUp(); // 升级时全属性+1，并恢复满HP/SP

private:
    int equipmentBonus[4]; // 四项属性加成
    void recalcStatusFlags(); // 根据activeStatusEffects更新statusFlags
};