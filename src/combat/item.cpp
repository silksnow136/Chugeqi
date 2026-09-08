#include "item.h"
#include <algorithm>

Item::Item(const std::string& id, const std::string& name, const std::string& description, int price,
           const std::string& category)
    : id(id), name(name), description(description), price(price), category(category) {}

const std::string& Item::getId() const { return id; }
const std::string& Item::getName() const { return name; }
const std::string& Item::getDescription() const { return description; }
int Item::getPrice() const { return price; }
const std::string& Item::getCategory() const { return category; }

Consumable::Consumable(const std::string& id, const std::string& name, const std::string& description, int price,
                       int healHP, int healSP, int statBonus, int statIndex, int duration,
                       const std::string& category)
    : Item(id, name, description, price, category), healHP(healHP), healSP(healSP),
      statBonus(statBonus), statIndex(statIndex), duration(duration) {}

int Consumable::getHealHP() const { return healHP; }
int Consumable::getHealSP() const { return healSP; }
int Consumable::getStatBonus() const { return statBonus; }
int Consumable::getStatIndex() const { return statIndex; }
int Consumable::getDuration() const { return duration; }

Equipment::Equipment(const std::string& id, const std::string& name, const std::string& description, int price,
                     EquipmentSlot slot, const int bonus[4], const std::string& category)
    : Item(id, name, description, price, category), slot(slot) {
    std::copy(bonus, bonus + 4, statBonus);
}

EquipmentSlot Equipment::getSlot() const { return slot; }
void Equipment::getStatBonus(int out[4]) const {
    std::copy(statBonus, statBonus + 4, out);
}