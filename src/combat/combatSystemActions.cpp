#include "combatSystem.h"
#include "core/console.h"
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// 行动执行
// ---------------------------------------------------------------------------

void CombatSystem::performAttack(Combatant* attacker, Combatant* target, bool isNormalAttack) {
    // 命中判定（基于敏捷）
    float baseHit = isNormalAttack ? 0.95f : 0.90f;
    float hitRate = calculateHitRate(baseHit, attacker->getEffectiveStat(3), target->getEffectiveStat(3));
    if (roll(100) >= static_cast<int>(hitRate * 100)) {
        addLog(attacker->getName() + " 攻击 " + target->getName() + "，但未命中！");
        return; // 未命中
    }

    // 伤害 = 力量 * 威力 - 防御（普通攻击威力固定为 10）
    int damage = calculateDamage(attacker->getEffectiveStat(0), 10, target->getEffectiveStat(2));
    target->takeDamage(damage);
    addLog(attacker->getName() + " 攻击 " + target->getName() + "，造成 " + std::to_string(damage) + " 点伤害。");
}

void CombatSystem::performSkill(Combatant* user, SkillBase* skill, std::vector<Combatant*>& targets) {
    // 检查并扣除 SP
    if (user->getSP() < skill->getCost()) {
        addLog(user->getName() + " 的 SP 不足，无法使用「" + skill->getName() + "」。");
        std::cout << user->getName() << " SP不足！" << std::endl;
        console::pause();
        return;
    }
    user->restoreSP(-skill->getCost());

    if (auto* dmg = dynamic_cast<DamageSkill*>(skill)) {
        // 伤害技能
        for (auto* target : targets) {
            float hitRate = calculateHitRate(dmg->getHitRate(), user->getEffectiveStat(3), target->getEffectiveStat(3));
            if (roll(100) >= static_cast<int>(hitRate * 100)) {
                addLog(user->getName() + " 的「" + skill->getName() + "」未命中 " + target->getName() + "。");
                continue; // 未命中
            }
            int damage = dmg->calculateDamage(user->getEffectiveStat(0), target->getEffectiveStat(2));
            target->takeDamage(damage);
            addLog(user->getName() + " 对 " + target->getName() + " 使用「" + skill->getName() + "」，造成 " + std::to_string(damage) + " 点伤害。");
            // 命中后附加技能附带的状态效果（skill.json 未配置则不附加）
            StatusEffect se = dmg->getStatusEffect();
            if (se != StatusEffect::None && target->isAlive()) {
                target->addStatusEffect(se, defaultStatusDuration(se));
                addLog(target->getName() + " 陷入「" + statusName(se) + "」状态！");
            }
        }
    } else if (auto* heal = dynamic_cast<HealSkill*>(skill)) {
        // 治疗技能
        for (auto* target : targets) {
            target->heal(heal->getHealAmount());
            addLog(user->getName() + " 对 " + target->getName() + " 使用「" + skill->getName() + "」，恢复 " + std::to_string(heal->getHealAmount()) + " 点 HP。");
        }
    } else if (auto* charge = dynamic_cast<ChargingSkill*>(skill)) {
        // 充能技能：给自己附加充能状态（倍率与持续回合取自技能配置）
        user->addStatusEffect(StatusEffect::Charge, charge->getDuration(),
                              charge->getTargetStat(), charge->getMultiplier());
        addLog(user->getName() + " 使用「" + skill->getName() + "」，进入充能状态！");
    } else {
        // 其他未实现的技能类型
        std::cout << "该技能类型：(未实现)" << std::endl;
        console::pause();
    }
}

void CombatSystem::performItem(Combatant* user, const std::string& itemId) {
    if (config.disableItems) {
        addLog("此战斗禁止使用道具！");
        std::cout << "此战斗禁止使用道具！" << std::endl;
        console::pause();
        return;
    }
    if (itemPool == nullptr) {
        addLog(user->getName() + " 的道具数据未加载，无法使用道具。");
        return;
    }
    if (!user->hasItem(itemId)) {
        addLog(user->getName() + " 没有这个道具！");
        return;
    }
    auto it = itemPool->find(itemId);
    if (it == itemPool->end()) {
        addLog("找不到道具数据：" + itemId);
        return;
    }
    const Consumable* c = dynamic_cast<const Consumable*>(it->second.get());
    if (c == nullptr || (c->getHealHP() <= 0 && c->getHealSP() <= 0)) {
        addLog("该道具无法在战斗中使用。");
        return;
    }
    bool used = false;
    if (c->getHealHP() > 0) {
        user->heal(c->getHealHP());
        addLog(user->getName() + " 使用「" + c->getName() + "」，恢复 "
               + std::to_string(c->getHealHP()) + " 点 HP！");
        used = true;
    }
    if (c->getHealSP() > 0) {
        user->restoreSP(c->getHealSP());
        addLog(user->getName() + " 使用「" + c->getName() + "」，恢复 "
               + std::to_string(c->getHealSP()) + " 点 SP！");
        used = true;
    }
    if (used) {
        user->consumeItem(itemId, 1);
    }
}

// 战斗中道具菜单：列出背包中可用的药品，输入 use+编号 使用（作用于使用者自身）
// 返回 true 表示成功使用了一个道具（本回合行动结束）
bool CombatSystem::useItemInBattle(Combatant* actor) {
    if (itemPool == nullptr) {
        std::cout << "道具数据未加载，无法使用。" << std::endl;
        console::pause();
        return false;
    }
    // 收集背包中可用药品（potion 且数量 > 0）
    std::vector<std::pair<std::string, const Consumable*>> potions;
    const auto& inv = actor->getInventory();
    for (const auto& pair : inv) {
        if (pair.second <= 0) continue;
        auto it = itemPool->find(pair.first);
        if (it == itemPool->end()) continue;
        const Consumable* c = dynamic_cast<const Consumable*>(it->second.get());
        if (c == nullptr || c->getCategory() != "potion") continue;
        if (c->getHealHP() <= 0 && c->getHealSP() <= 0) continue;
        potions.push_back(std::make_pair(pair.first, c));
    }
    if (potions.empty()) {
        std::cout << "背包中没有可用的药品。" << std::endl;
        console::pause();
        return false;
    }
    std::cout << "选择要使用的药品（输入 use+编号，直接回车返回）：" << std::endl;
    for (size_t i = 0; i < potions.size(); i++) {
        const Consumable* c = potions[i].second;
        std::cout << i + 1 << ". " << c->getName()
                  << "（数量 " << inv.at(potions[i].first) << "）";
        if (c->getHealHP() > 0) std::cout << "  HP+" << c->getHealHP();
        if (c->getHealSP() > 0) std::cout << "  SP+" << c->getHealSP();
        std::cout << std::endl;
    }
    std::cout << "> ";
    std::string cmd = console::readLine();
    if (cmd.empty()) return false; // 直接回车返回主菜单
    // 解析 use+编号（支持 use3 / use 3 / 直接数字）
    if (cmd[0] == 'u' || cmd[0] == 'U') {
        cmd = cmd.substr(3);
        while (!cmd.empty() && (cmd[0] == ' ' || cmd[0] == '\t')) cmd.erase(cmd.begin());
    }
    int idx = 0;
    try {
        idx = std::stoi(cmd);
    } catch (...) {
        std::cout << "无效输入，请输入 use+编号。" << std::endl;
        console::pause();
        return false;
    }
    if (idx < 1 || idx > static_cast<int>(potions.size())) {
        std::cout << "无效编号。" << std::endl;
        console::pause();
        return false;
    }
    performItem(actor, potions[idx - 1].first);
    return true;
}

void CombatSystem::attemptRun(Combatant* runner) {
    if (config.disableRun) {
        addLog("此战斗禁止逃跑！");
        std::cout << "此战斗禁止逃跑！" << std::endl;
        console::pause();
        return;
    }

    // 逃跑成功率：基于自身敏捷与敌方敏捷之和
    int selfAgi = runner->getEffectiveStat(3);
    int totalEnemyAgi = 0;
    for (auto* e : getAliveEnemies()) totalEnemyAgi += e->getEffectiveStat(3);

    if (totalEnemyAgi == 0 || roll(100) < (selfAgi * 100 / (selfAgi + totalEnemyAgi))) {
        addLog(runner->getName() + " 成功逃跑！");
        std::cout << runner->getName() << " 成功逃跑！" << std::endl;
        console::pause();
        battleEnded = true;
        playerWon = false;
        playerEscaped = true;
        return;
    }

    addLog(runner->getName() + " 逃跑失败！");
    std::cout << runner->getName() << " 逃跑失败！" << std::endl;
    console::pause();
}
