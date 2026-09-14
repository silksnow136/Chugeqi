#include "PharManager.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "core/console.h"
using namespace std;

PharManager::PharManager(const ItemPool& itemPool, int& gold)
    : itemPool(itemPool), gold(gold)
{
}

// 收集药店可出售的药品（消耗品、分类为 potion、价格>0），
// 按价格升序、ID 升序稳定排序，保证编号确定
static std::vector<const Consumable*> collectShopMedicines(const ItemPool& itemPool) {
    std::vector<const Consumable*> list;
    for (const auto& pair : itemPool) {
        const Consumable* m = dynamic_cast<const Consumable*>(pair.second.get());
        if (m == nullptr || m->getCategory() != "potion") continue;
        if (m->getPrice() <= 0) continue; // 剧情拾取类药品不在药店出售
        list.push_back(m);
    }
    std::sort(list.begin(), list.end(), [](const Consumable* a, const Consumable* b) {
        if (a->getPrice() != b->getPrice()) return a->getPrice() < b->getPrice();
        return a->getId() < b->getId();
    });
    return list;
}

// 收集玩家背包中的药品（含数量），按 ID 升序保证编号确定
static std::vector<std::pair<const Consumable*, int>> collectBagMedicines(
    const ItemPool& itemPool, const Combatant& player) {
    std::vector<std::pair<const Consumable*, int>> list;
    for (const auto& kv : player.getInventory()) {
        if (kv.second <= 0) continue;
        auto it = itemPool.find(kv.first);
        if (it == itemPool.end()) continue;
        const Consumable* m = dynamic_cast<const Consumable*>(it->second.get());
        if (m == nullptr || m->getCategory() != "potion") continue;
        list.push_back({ m, kv.second });
    }
    std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) {
        return a.first->getId() < b.first->getId();
    });
    return list;
}

// 打印药品的恢复效果说明
static void printMedicineEffect(const Consumable* m) {
    if (m->getHealHP() > 0) cout << "（恢复" << m->getHealHP() << "点生命）";
    if (m->getHealSP() > 0) cout << "（恢复" << m->getHealSP() << "点内力）";
}

// 读取一行纯数字输入，返回解析出的非负整数；非法输入或取消返回 -1
static int readNumber() {
    string line;
    if (!getline(cin, line)) return -1;
    if (line.empty()) return -1;
    int n = 0;
    for (char ch : line) {
        if (ch < '0' || ch > '9') return -1;
        n = n * 10 + (ch - '0');
    }
    return n;
}

// 进入药房系统
void PharManager::phar(Combatant& player)
{
    bool running = true;

    while (running)
    {
        console::clearScreen();

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
            cout << "\n按回车返回";
            getline(cin, command);
        }
        else if (command == "2")
        {
            useItem(player);
            cout << "\n按回车返回";
            getline(cin, command);
        }
        else if (command == "3")
        {
            showMedicineBag(player);
            cout << "\n按回车返回";
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
    cout << "\n========== 药品一览 ==========\n";

    auto medicines = collectShopMedicines(itemPool);

    if (medicines.empty())
    {
        cout << "目前没有可购买的药品。\n";
        return;
    }

    for (const Consumable* m : medicines)
    {
        cout << "  " << m->getName() << "  " << m->getPrice() << " 金币";
        printMedicineEffect(m);
        cout << "\n";
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
    auto medicines = collectShopMedicines(itemPool);

    if (medicines.empty())
    {
        cout << "目前没有可购买的药品。\n";
        return;
    }

    cout << "\n========== 购买药品 ==========\n";
    cout << "当前金币：" << gold << "\n";
    for (size_t i = 0; i < medicines.size(); i++)
    {
        const Consumable* m = medicines[i];
        cout << " " << (i + 1) << ". " << m->getName()
             << "  " << m->getPrice() << " 金币";
        printMedicineEffect(m);
        cout << "\n";
    }
    cout << " 0. 返回\n";
    cout << "请输入编号购买（0 返回）：";

    int n = readNumber();
    if (n <= 0)
    {
        cout << "已取消购买。\n";
        return;
    }
    if (n > static_cast<int>(medicines.size()))
    {
        cout << "编号无效！\n";
        return;
    }

    const Consumable* medicine = medicines[n - 1];
    int price = medicine->getPrice();

    if (gold < price)
    {
        cout << "金币不足！\n";
        return;
    }

    // 加入玩家背包
    player.addItem(medicine->getId(), 1);

    // 扣除金币
    gold -= price;

    cout << "\n购买成功！\n";
    cout << "获得：" << medicine->getName() << "\n";
    cout << "花费：" << price << " 金币\n";
    cout << "剩余金币：" << gold << "\n";
}

void PharManager::useItem(Combatant& player)
{
    auto medicines = collectBagMedicines(itemPool, player);

    if (medicines.empty())
    {
        cout << "\n背包中没有药品。\n";
        return;
    }

    cout << "\n========== 使用药品 ==========\n";
    for (size_t i = 0; i < medicines.size(); i++)
    {
        const Consumable* m = medicines[i].first;
        cout << " " << (i + 1) << ". " << m->getName()
             << " ×" << medicines[i].second;
        printMedicineEffect(m);
        cout << "\n";
    }
    cout << " 0. 返回\n";
    cout << "请输入编号使用（0 返回）：";

    int n = readNumber();
    if (n <= 0)
    {
        cout << "已取消使用。\n";
        return;
    }
    if (n > static_cast<int>(medicines.size()))
    {
        cout << "编号无效！\n";
        return;
    }

    const Consumable* medicine = medicines[n - 1].first;

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
    player.consumeItem(medicine->getId(), 1);

    cout << "使用了：" << medicine->getName() << "\n";
}

void PharManager::showMedicineBag(Combatant& player)
{
    cout << "\n========== 我的药品 ==========\n";

    auto medicines = collectBagMedicines(itemPool, player);

    if (medicines.empty())
    {
        cout << "背包中没有药品。\n";
        return;
    }

    for (size_t i = 0; i < medicines.size(); i++)
    {
        const Consumable* m = medicines[i].first;
        cout << " " << (i + 1) << ". " << m->getName()
             << " ×" << medicines[i].second;
        printMedicineEffect(m);
        cout << "\n";
    }
}
