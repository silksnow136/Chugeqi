#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// 物品基类
class Item {
protected:
    std::string id;
    std::string name;
    std::string description;
    int price;
public:
    Item(const std::string& id, const std::string& name, const std::string& description, int price);
    virtual ~Item() = default;
    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    int getPrice() const;
};

// 消耗品（使用效果类似技能）
class Consumable : public Item {
private:
    int healHP;        // 恢复HP量，0表示无
    int healSP;        // 恢复SP量，0表示无
    int statBonus;     // 临时属性加成（仅本场战斗有效），0表示无
    int statIndex;     // 对应strength(0), magic(1), endurance(2), agility(3)
    int duration;      // 持续回合数，0表示立即生效
public:
    Consumable(const std::string& id, const std::string& name, const std::string& description, int price,
               int healHP, int healSP, int statBonus = 0, int statIndex = -1, int duration = 0);
    int getHealHP() const;
    int getHealSP() const;
    int getStatBonus() const;
    int getStatIndex() const;
    int getDuration() const;
};

// 装备（盔甲、武器、载具/鞋子、配饰）
enum class EquipmentSlot {
    Armor,
    Weapon,
    Shoes,   // 或载具
    Accessory
};

class Equipment : public Item {
private:
    EquipmentSlot slot;
    int statBonus[4];   // 对strength, magic, endurance, agility的加成
public:
    Equipment(const std::string& id, const std::string& name, const std::string& description, int price,
              EquipmentSlot slot, const int bonus[4]);
    EquipmentSlot getSlot() const;
    void getStatBonus(int out[4]) const;
};

// 物品池：由 DataLoader 长期持有，唯一拥有 Item 对象的所有权。
using ItemPool = std::unordered_map<std::string, std::unique_ptr<Item>>;