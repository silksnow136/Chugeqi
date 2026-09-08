#pragma once

#include <string>
#include "combat/character.h"
#include "combat/item.h"

using namespace std;

// 药店管理
class PharManager
{
private:
    // 道具池，来自 DataLoader
    const ItemPool& itemPool;

    // 玩家当前金币
    int& gold;

public:

    // 创建药店管理器
    PharManager(const ItemPool& itemPool, int& gold);

    // 进入药房系统
    void phar(Combatant& player);

private:

    // 显示药店中的药品
    void showShop();

    // 购买药品
    void buyItem(Combatant& player);

    // 使用药品
    void useItem(Combatant& player);

    // 查看玩家背包中的药品
    void showMedicineBag(Combatant& player);

    // 查找指定药品
    const Consumable* findMedicine(const string& itemId) const;
};