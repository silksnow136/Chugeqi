#include "item.h"
#include <algorithm>

Item::Item(const std::string& id, const std::string& name, const std::string& description, int price)
    : id(id), name(name), description(description), price(price) {}

const std::string& Item::getId() const { return id; }
const std::string& Item::getName() const { return name; }
const std::string& Item::getDescription() const { return description; }
int Item::getPrice() const { return price; }

Consumable::Consumable(const std::string& id, const std::string& name, const std::string& description, int price,
                       int healHP, int healSP, int statBonus, int statIndex, int duration)
    : Item(id, name, description, price), healHP(healHP), healSP(healSP),
      statBonus(statBonus), statIndex(statIndex), duration(duration) {}

int Consumable::getHealHP() const { return healHP; }
int Consumable::getHealSP() const { return healSP; }
int Consumable::getStatBonus() const { return statBonus; }
int Consumable::getStatIndex() const { return statIndex; }
int Consumable::getDuration() const { return duration; }

Equipment::Equipment(const std::string& id, const std::string& name, const std::string& description, int price,
                     EquipmentSlot slot, const int bonus[4])
    : Item(id, name, description, price), slot(slot) {
    std::copy(bonus, bonus + 4, statBonus);
}

EquipmentSlot Equipment::getSlot() const { return slot; }
void Equipment::getStatBonus(int out[4]) const {
    std::copy(statBonus, statBonus + 4, out);
}