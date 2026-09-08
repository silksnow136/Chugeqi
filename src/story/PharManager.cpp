#include "PharManager.h"
#include <iostream>
#include<core/console.h>
using namespace std;

PharManager::PharManager(const ItemPool& itemPool, int& gold)
    : itemPool(itemPool), gold(gold)
{
}

// 进入药房系统
void PharManager::phar(Combatant& player)
{
    bool running = true;

    while (running)
    {
        system("cls");

        cout << "========================================\n";
        cout << "                药       店              \n";
        cout << "========================================\n";

        cout << "当前金币：" << gold << "\n\n";
        showShop();
        cout << "1. 购买药品\n";
        cout << "2. 使用药品\n";
        cout << "3. 查看背包\n";
        cout << "4. 离开药店\n";

        cout << "\n> ";

        string command;
        getline(cin, command);

       if (command == "1")
        {
            buyItem(player);

            cout << "\n按任意键返回";
            getline(cin, command);
        }
        else if (command == "2")
        {
            useItem(player);

            cout << "\n按任意键返回";
            getline(cin, command);
        }
        else if (command == "3")
        {
            showMedicineBag(player);

            cout << "\n按任意键返回";
            getline(cin, command);
        }
        else if (command == "4")
        {
            running = false;
        }
        else
        {
            cout << "无效指令！\n";
            console::sleep(500);
        }
    }
}

void PharManager::showShop()
{
    cout << "\n========== 药品列表 ==========\n";

    bool found = false;

    for (const auto& pair : itemPool)
    {
        const Item* item = pair.second.get();

        // 判断是不是消耗品
        const Consumable* medicine =
            dynamic_cast<const Consumable*>(item);

        if (medicine == nullptr)
            continue;

        found = true;

        cout << "\nID：" << medicine->getId() << "\n";
        cout << "名称：" << medicine->getName() << "\n";
        cout << "说明：" << medicine->getDescription() << "\n";
        cout << "价格：" << medicine->getPrice() << "\n";

        if (medicine->getHealHP() > 0)
        {
            cout << "恢复 HP："
                << medicine->getHealHP() << "\n";
        }

        if (medicine->getHealSP() > 0)
        {
            cout << "恢复 SP："
                << medicine->getHealSP() << "\n";
        }

        cout << "------------------------------\n";
    }

    if (!found)
    {
        cout << "目前没有可购买的药品。\n";
    }
}

const Consumable* PharManager::findMedicine(
    const string& itemId) const
{
    auto it = itemPool.find(itemId);

    if (it == itemPool.end())
    {
        return nullptr;
    }

    const Item* item = it->second.get();

    return dynamic_cast<const Consumable*>(item);
}

void PharManager::buyItem(Combatant& player)
{
    //showShop();

    cout << "\n请输入要购买的药品 ID：";

    string itemId;
    getline(cin, itemId);

    const Consumable* medicine = findMedicine(itemId);

    if (medicine == nullptr)
    {
        cout << "没有找到这个药品。\n";
        return;
    }

    int price = medicine->getPrice();

    if (gold < price)
    {
        cout << "金币不足！\n";
        return;
    }

    // 加入玩家背包
    player.addItem(itemId, 1);

    // 扣除金币
    gold -= price;

    cout << "\n购买成功！\n";
    cout << "获得：" << medicine->getName() << "\n";
    cout << "花费：" << price << " 金币\n";
    cout << "剩余金币：" << gold << "\n";
}

void PharManager::useItem(Combatant& player)
{
    showMedicineBag(player);

    cout << "\n请输入要使用的药品 ID：";

    string itemId;
    getline(cin, itemId);

    if (!player.hasItem(itemId))
    {
        cout << "你没有这个药品。\n";
        return;
    }

    const Consumable* medicine = findMedicine(itemId);

    if (medicine == nullptr)
    {
        cout << "找不到该药品的数据。\n";
        return;
    }

    // 恢复 HP
    if (medicine->getHealHP() > 0)
    {
        player.heal(medicine->getHealHP());

        cout << "HP +" << medicine->getHealHP() << "\n";
    }

    // 恢复 SP
    if (medicine->getHealSP() > 0)
    {
        player.restoreSP(medicine->getHealSP());

        cout << "SP +" << medicine->getHealSP() << "\n";
    }

    // 消耗一个药品
    player.consumeItem(itemId, 1);

    cout << "使用了：" << medicine->getName() << "\n";
}

void PharManager::showMedicineBag(Combatant& player)
{
    cout << "\n========== 我的药品 ==========\n";

    const auto& inventory = player.getInventory();

    bool found = false;

    for (const auto& pair : inventory)
    {
        const string& itemId = pair.first;
        int count = pair.second;

        if (count <= 0)
            continue;

        const Consumable* medicine = findMedicine(itemId);

        if (medicine == nullptr)
            continue;

        found = true;

        cout << "ID：" << itemId
            << " | "
            << medicine->getName()
            << " | 数量：" << count
            << "\n";
    }

    if (!found)
    {
        cout << "背包中没有药品。\n";
    }
}