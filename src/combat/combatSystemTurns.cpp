#include "combatSystem.h"
#include "core/console.h"
#include <iostream>

// ---------------------------------------------------------------------------
// 回合处理
// ---------------------------------------------------------------------------

void CombatSystem::processPlayerTurn() {
    if (playerAiAssisted) { processAllyAITurn(player); return; } // AI 托管
    manualTurn(player, 5);                                       // 手动：含切换托管项
}

void CombatSystem::processCompanionTurn(Combatant* companion) {
    if (playerAiAssisted || companionAiAssisted) { processAllyAITurn(companion); return; } // 全员托管 或 同伴独立托管
    manualTurn(companion, 3); // 手动（含托管选项）
}

// 手动回合：玩家(maxChoice=5，含全员托管切换)与同伴(maxChoice=3，含同伴托管切换)共用
void CombatSystem::manualTurn(Combatant* actor, int maxChoice) {
    // 眩晕：跳过本回合行动（在目标自己的回合被跳过时消费掉眩晕）
    if (actor->hasStatusEffect(StatusEffect::Stun)) {
        actor->removeStatusEffect(StatusEffect::Stun);
        addLog(actor->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        console::pause();
        return;
    }
    const bool isPlayer = (maxChoice >= 5);
    const bool canRun = isPlayer && !config.disableRun;
    while (true) {
        displayBattle(); // 行动前刷新一次界面
        if (isPlayer) {
            std::cout << "[1]攻击  [2]技能  [3]道具";
            if (canRun) std::cout << "  [4]逃跑";
            std::cout << "  [" << (canRun ? 5 : 4) << "]"
                      << (playerAiAssisted ? "关闭全员AI托管" : "开启全员AI托管") << std::endl;
        } else {
            std::cout << actor->getName() << " 的行动：[1]攻击  [2]技能  [3]"
                      << (companionAiAssisted ? "关闭AI托管" : "AI托管") << std::endl;
        }

        int choice = readMenuChoice(1, canRun ? 5 : (isPlayer ? 4 : 3));
        switch (choice) {
            case 1: { // 普通攻击：手动选择目标
                Combatant* t = selectTarget(getAliveEnemies(), "选择攻击目标：");
                if (!t) continue; // 取消，返回主菜单
                performAttack(actor, t, true);
                break;
            }
            case 2: { // 技能：单体手动选择目标
                SkillBase* s = selectSkill(actor);
                if (!s) continue; // 取消，返回主菜单
                std::vector<Combatant*> targets;
                if (dynamic_cast<ChargingSkill*>(s) != nullptr) {
                    // 充能技能：目标固定为施法者自己
                    targets.push_back(actor);
                } else {
                    targets = buildSkillTargets(getAliveAllies(), getAliveEnemies(), s);
                }
                if (targets.empty()) continue; // 取消，返回主菜单
                performSkill(actor, s, targets);
                break;
            }
            case 3:
                if (isPlayer) { // 玩家：道具（使用药品，指令 use+编号）
                    if (config.disableItems) {
                        addLog("此战斗禁止使用道具！");
                        std::cout << "此战斗禁止使用道具！" << std::endl;
                        console::pause();
                        continue;
                    }
                    if (useItemInBattle(actor)) {
                        displayBattle(); // 属性变动后立即刷新
                        console::pause();
                        return;          // 使用道具算一次行动，结束本回合
                    }
                    continue; // 未使用（取消/无效输入），返回主菜单
                }
                // 同伴：切换同伴 AI 托管，开启后立即由 AI 接管本回合
                companionAiAssisted = !companionAiAssisted;
                if (companionAiAssisted) {
                    addLog(std::string(actor->getName()) + " 进入了 AI 托管。");
                    processAllyAITurn(actor);
                    return;
                }
                addLog(std::string(actor->getName()) + " 退出了 AI 托管。");
                std::cout << "已关闭 " << actor->getName() << " 的 AI 托管。" << std::endl;
                console::pause();
                continue;
            case 4:
                if (isPlayer && !canRun) {
                    // 逃跑被禁用：第 4 项即全员 AI 托管切换
                    playerAiAssisted = !playerAiAssisted;
                    if (playerAiAssisted) {
                        addLog("全体我方角色进入了 AI 托管。");
                        processAllyAITurn(actor);
                        return;
                    }
                    companionAiAssisted = false;
                    addLog("全体我方角色退出了 AI 托管。");
                    std::cout << "已关闭全员 AI 托管。" << std::endl;
                    console::pause();
                    continue;
                }
                attemptRun(actor); // 逃跑（仅玩家）
                return;
            case 5: // 切换全员 AI 托管：开启后立即由 AI 接管本回合
                playerAiAssisted = !playerAiAssisted;
                if (playerAiAssisted) {
                    addLog("全体我方角色进入了 AI 托管。");
                    processAllyAITurn(actor); // actor 必为 player（仅玩家菜单含此项）
                    return;
                }
                companionAiAssisted = false; // 全员关闭时一并清除同伴托管
                addLog("全体我方角色退出了 AI 托管。");
                std::cout << "已关闭全员 AI 托管。" << std::endl;
                console::pause();
                continue;
        }
        displayBattle(); // HP/SP 变动后立即刷新显示
        console::pause();
        return;
    }
}

// AI 行动后的等待：任意键继续；ESC 立即退出所有托管模式，下一步即恢复手操
void CombatSystem::aiPause() {
    if (console::pauseEsc()) {
        if (playerAiAssisted) {
            playerAiAssisted = false;
            companionAiAssisted = false; // 一并清除同伴托管
            addLog("全体我方角色退出了 AI 托管（ESC）。");
        } else if (companionAiAssisted) {
            companionAiAssisted = false;
            addLog("同伴退出了 AI 托管（ESC）。");
        }
    }
}

void CombatSystem::processEnemyTurn(Combatant* enemy) {
    // 敌人 AI：眩晕跳过；否则按策略选择普攻或技能，集火我方残血
    if (enemy->hasStatusEffect(StatusEffect::Stun)) {
        enemy->removeStatusEffect(StatusEffect::Stun);
        addLog(enemy->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        aiPause();
        return;
    }

    auto targets = getAliveAllies();
    if (targets.empty()) return; // 无可攻击目标

    // 决策：有可用伤害技能且 SP 足够时，约 60% 概率用技能，否则普攻
    SkillBase* skill = chooseAISkill(enemy);
    if (skill) {
        // 全体技能自动选中全部我方，单体技能选 HP 最低者
        std::vector<Combatant*> skillTargets;
        if (skill->getScope() == AttackScope::All) {
            skillTargets = targets;
        } else {
            Combatant* t = chooseAITarget(enemy, targets);
            if (t) skillTargets.push_back(t);
        }
        if (!skillTargets.empty()) {
            displayBattle();
            performSkill(enemy, skill, skillTargets);
            displayBattle();
            aiPause();
            return;
        }
    }

    // 普通攻击：集火 HP 最低的我方
    Combatant* target = chooseAITarget(enemy, targets);
    if (!target) return;
    displayBattle();
    performAttack(enemy, target, true);
    displayBattle();
    aiPause();
}

// 我方 AI 托管回合：低血优先治疗，否则优先伤害技能，最后退回普攻
void CombatSystem::processAllyAITurn(Combatant* actor) {
    // 眩晕跳过
    if (actor->hasStatusEffect(StatusEffect::Stun)) {
        actor->removeStatusEffect(StatusEffect::Stun);
        addLog(actor->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        aiPause();
        return;
    }

    auto allies = getAliveAllies();   // 含 actor 自身，治疗可选
    auto enemies = getAliveEnemies();
    if (enemies.empty()) return;

    // 满血按最大生命估算；HP 低于 30% 视为残血
    int maxHp = actor->getMaxHP();
    bool lowHp = actor->getHP() < (maxHp * 3 / 10);

    // 1) 低血优先：找可用治疗技能
    if (lowHp) {
        for (auto* s : actor->getSkills()) {
            if (s->getCost() > actor->getSP()) continue;
            if (dynamic_cast<HealSkill*>(s) == nullptr) continue;
            // 治疗目标：全体技能→全队，单体技能→HP 最低的我方（含自己）
            std::vector<Combatant*> tgts;
            if (s->getScope() == AttackScope::All) {
                tgts = allies;
            } else {
                Combatant* t = chooseAITarget(actor, allies);
                if (t) tgts.push_back(t);
            }
            if (!tgts.empty()) {
                displayBattle();
                performSkill(actor, s, tgts);
                displayBattle();
                aiPause();
                return;
            }
        }
    }

    // 2) 优先伤害技能：找 SP 足够的 DamageSkill
    for (auto* s : actor->getSkills()) {
        if (s->getCost() > actor->getSP()) continue;
        if (dynamic_cast<DamageSkill*>(s) == nullptr) continue;
        std::vector<Combatant*> tgts;
        if (s->getScope() == AttackScope::All) {
            tgts = enemies;
        } else {
            Combatant* t = chooseAITarget(actor, enemies);
            if (t) tgts.push_back(t);
        }
        if (!tgts.empty()) {
            displayBattle();
            performSkill(actor, s, tgts);
            displayBattle();
            aiPause();
            return;
        }
    }

    // 3) 退回普攻：集火 HP 最低的敌方
    Combatant* t = chooseAITarget(actor, enemies);
    if (!t) return;
    displayBattle();
    performAttack(actor, t, true);
    displayBattle();
    aiPause();
}

// ---------------------------------------------------------------------------
// AI 决策
// ---------------------------------------------------------------------------

SkillBase* CombatSystem::chooseAISkill(Combatant* ai) {
    // 敌人进攻型 AI：只考虑伤害技能，且 SP 需足够
    std::vector<SkillBase*> usable;
    for (auto* s : ai->getSkills()) {
        if (s->getCost() > ai->getSP()) continue;          // SP 不足
        if (dynamic_cast<DamageSkill*>(s) == nullptr) continue; // 只选伤害技能
        usable.push_back(s);
    }
    if (usable.empty()) return nullptr;                     // 无可用技能 → 走普攻
    // 约 60% 概率施放技能，否则普攻（避免每次都放技能）
    if (roll(100) >= 60) return nullptr;
    return usable[roll(static_cast<int>(usable.size()))];   // 候选中随机
}

Combatant* CombatSystem::chooseAITarget(Combatant* ai, const std::vector<Combatant*>& potentialTargets) {
    if (potentialTargets.empty()) return nullptr;
    // 策略：集火 HP 最低的我方（平手时取首个）
    Combatant* best = potentialTargets[0];
    for (auto* c : potentialTargets) {
        if (c->getHP() < best->getHP()) best = c;
    }
    return best;
}
