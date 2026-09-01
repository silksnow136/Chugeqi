#include "combatSystem.h"
#include "console.h"
#include <iostream>
#include <random>

// ---------------------------------------------------------------------------
// 文件内部辅助函数（不对外暴露）
// ---------------------------------------------------------------------------

// 读取 [min, max] 范围内的数字键，返回对应的整数；
// allowBack 为 true 时，按 0 或 ESC 返回 -1（表示返回上一级菜单）。
static int readMenuChoice(int min, int max, bool allowBack = false) {
    while (true) {
        int key = console::readKey();
        if (allowBack && (key == 27 || key == '0')) return -1;
        if (key >= '0' + min && key <= '0' + max) {
            return key - '0';
        }
    }
}

// 显示技能列表并让玩家选择，返回选中的技能（无技能时返回 nullptr）
static SkillBase* selectSkill(Combatant* actor) {
    const auto& skills = actor->getSkills();
    if (skills.empty()) {
        std::cout << "没有可用的技能。" << std::endl;
        console::pause();
        return nullptr;
    }
    std::cout << "选择技能：" << std::endl;
    for (size_t i = 0; i < skills.size(); i++) {
        std::cout << i + 1 << ". " << skills[i]->getName()
                  << " (SP:" << skills[i]->getCost() << ")" << std::endl;
    }
    std::cout << "[0]返回" << std::endl;
    int choice = readMenuChoice(1, static_cast<int>(skills.size()), true);
    if (choice < 0) return nullptr;
    return skills[choice - 1];
}

// 手动选择目标：列出候选并读取选择（仅一个候选时直接返回，无需选择）
static Combatant* selectTarget(const std::vector<Combatant*>& candidates, const std::string& prompt) {
    if (candidates.empty()) return nullptr;
    if (candidates.size() == 1) return candidates[0];

    std::cout << prompt << std::endl;
    for (size_t i = 0; i < candidates.size(); i++) {
        std::cout << i + 1 << ". " << candidates[i]->getName()
                  << "  HP:" << candidates[i]->getHP() << std::endl;
    }
    std::cout << "[0]返回" << std::endl;
    int choice = readMenuChoice(1, static_cast<int>(candidates.size()), true);
    if (choice < 0) return nullptr;
    return candidates[choice - 1];
}

// 构建技能目标列表：单体技能手动选择目标，全体技能自动选中全部
static std::vector<Combatant*> buildSkillTargets(const std::vector<Combatant*>& allies,
                                                 const std::vector<Combatant*>& enemies,
                                                 SkillBase* skill) {
    bool isHeal = dynamic_cast<HealSkill*>(skill) != nullptr;
    if (skill->getScope() == AttackScope::All) {
        return isHeal ? allies : enemies;
    }
    Combatant* t = selectTarget(isHeal ? allies : enemies,
                                isHeal ? "选择治疗目标：" : "选择攻击目标：");
    if (!t) return {};
    return {t};
}

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------

CombatSystem::CombatSystem(Combatant* player, std::vector<Combatant*> companions,
                           std::vector<Combatant*> enemies,
                           const CombatConfig& config)
    : player(player), companions(companions), enemies(enemies), config(config) {
    std::random_device rd;
    rng.seed(rd());
}

CombatSystem::~CombatSystem() {
    // 不负责释放外部对象（player / companions / enemies 由调用方管理）
}

// ---------------------------------------------------------------------------
// 战斗主流程
// ---------------------------------------------------------------------------

bool CombatSystem::startBattle() {
    console::init();
    battleEnded = false;
    playerWon = false;

    // 主循环：我方（玩家 + 同伴）→ 敌方（未实现，不会回击）
    while (!battleEnded) {
        // 玩家回合（手动，内部会刷新界面）
        if (player->isAlive()) {
            processPlayerTurn();
        }
        if (getAliveEnemies().empty()) { battleEnded = true; playerWon = true; break; }

        // 同伴回合
        // 防御：同伴全灭时清除同伴托管开关
        if (companionAiAssisted) {
            bool anyAlive = false;
            for (auto* c : companions) { if (c->isAlive()) { anyAlive = true; break; } }
            if (!anyAlive) companionAiAssisted = false;
        }
        for (auto* companion : companions) {
            if (companion->isAlive()) {
                processCompanionTurn(companion);
            }
        }
        if (getAliveEnemies().empty()) { battleEnded = true; playerWon = true; break; }

        // 敌方回合：敌人 AI 回击
        for (auto* enemy : getAliveEnemies()) {
            processEnemyTurn(enemy);
        }
        // 我方全灭 → 失败结束
        if (getAliveAllies().empty()) { battleEnded = true; playerWon = false; break; }
    }

    // 胜利结算：按敌人等级发放经验（用于演示存档持久化）
    if (playerWon) {
        int totalExp = 0;
        for (auto* e : enemies) totalExp += e->getLevel() * 10;
        player->addExp(totalExp);
        for (auto* c : companions) c->addExp(totalExp);
        addLog("队伍获得 " + std::to_string(totalExp) + " 点经验。");
    }

    // 战斗结束提示
    addLog(playerWon ? "战斗胜利！" : "战斗失败...");
    displayBattle();
    std::cout << (playerWon ? "战斗胜利！" : "战斗失败...") << std::endl;
    console::pause();
    return playerWon;
}

// ---------------------------------------------------------------------------
// 界面显示
// ---------------------------------------------------------------------------

void CombatSystem::addLog(const std::string& msg) {
    log.add(msg);
}

void CombatSystem::displayBattle() const {
    console::clearScreen();

    // 上方预留 ASCII 艺术空间
    std::cout << "   +-----------------------------------+" << std::endl;
    std::cout << "   |        [ASCII Art 预留区域]       |" << std::endl;
    std::cout << "   +-----------------------------------+" << std::endl;

    // 战斗日志
    std::cout << "=▽战斗日志▽============" << std::endl;
    std::cout << log.render();

    // 我方状态（托管中时显示标记；同伴全灭则不显示同伴托管标记）
    bool hasAliveCompanion = false;
    for (auto* c : companions) { if (c->isAlive()) { hasAliveCompanion = true; break; } }
    std::cout << "=▽我方▽================"
              << (playerAiAssisted ? "  [全员AI托管中，ESC退出]"
                                   : (companionAiAssisted && hasAliveCompanion ? "  [同伴AI托管中，ESC退出]" : ""))
              << std::endl;
    displayStatus(player);
    for (auto* companion : companions) {
        if (companion->isAlive()) {
            displayStatus(companion);
        }
    }

    // 敌方状态
    std::cout << "=▽敌方▽================" << std::endl;
    for (auto* e : getAliveEnemies()) {
        displayStatus(e);
    }
    std::cout << "----------------------------------------" << std::endl;
}

void CombatSystem::displayStatus(const Combatant* c) const {
    std::cout << c->getName() << "  Lv." << c->getLevel()
              << "  HP:" << c->getHP() << "  SP:" << c->getSP()
              << "  特殊状态：(未实现)" << std::endl;
}

// ---------------------------------------------------------------------------
// 回合处理
// ---------------------------------------------------------------------------

bool CombatSystem::processPlayerTurn() {
    if (playerAiAssisted) return processAllyAITurn(player); // AI 托管
    return manualTurn(player, 5);                           // 手动：含切换托管项
}

bool CombatSystem::processCompanionTurn(Combatant* companion) {
    if (playerAiAssisted || companionAiAssisted) return processAllyAITurn(companion); // 全员托管 或 同伴独立托管
    return manualTurn(companion, 3); // 手动（含托管选项）
}

// 手动回合：玩家(maxChoice=5，含全员托管切换)与同伴(maxChoice=3，含同伴托管切换)共用
bool CombatSystem::manualTurn(Combatant* actor, int maxChoice) {
    while (true) {
        displayBattle(); // 行动前刷新一次界面
        if (maxChoice >= 5)
            std::cout << "[1]攻击  [2]技能  [3]道具(未实现)  [4]逃跑  [5]"
                      << (playerAiAssisted ? "关闭全员AI托管" : "开启全员AI托管") << std::endl;
        else
            std::cout << actor->getName() << " 的行动：[1]攻击  [2]技能  [3]"
                      << (companionAiAssisted ? "关闭AI托管" : "AI托管") << std::endl;

        switch (readMenuChoice(1, maxChoice)) {
            case 1: { // 普通攻击：手动选择目标
                Combatant* t = selectTarget(getAliveEnemies(), "选择攻击目标：");
                if (!t) continue; // 取消，返回主菜单
                performAttack(actor, t, true);
                break;
            }
            case 2: { // 技能：单体手动选择目标
                SkillBase* s = selectSkill(actor);
                if (!s) continue; // 取消，返回主菜单
                auto targets = buildSkillTargets(getAliveAllies(), getAliveEnemies(), s);
                if (targets.empty()) continue; // 取消，返回主菜单
                performSkill(actor, s, targets);
                break;
            }
            case 3:
                if (maxChoice >= 5) { // 玩家：道具（未实现）
                    std::cout << "道具系统：(未实现)" << std::endl;
                    console::pause();
                    continue;
                }
                // 同伴：切换同伴 AI 托管，开启后立即由 AI 接管本回合
                companionAiAssisted = !companionAiAssisted;
                if (companionAiAssisted) {
                    addLog(std::string(actor->getName()) + " 进入了 AI 托管。");
                    return processAllyAITurn(actor);
                }
                addLog(std::string(actor->getName()) + " 退出了 AI 托管。");
                std::cout << "已关闭 " << actor->getName() << " 的 AI 托管。" << std::endl;
                console::pause();
                continue;
            case 4: // 逃跑（仅玩家）
                return attemptRun(actor);
            case 5: // 切换全员 AI 托管：开启后立即由 AI 接管本回合
                playerAiAssisted = !playerAiAssisted;
                if (playerAiAssisted) {
                    addLog("全体我方角色进入了 AI 托管。");
                    return processAllyAITurn(actor); // actor 必为 player（仅玩家菜单含此项）
                }
                companionAiAssisted = false; // 全员关闭时一并清除同伴托管
                addLog("全体我方角色退出了 AI 托管。");
                std::cout << "已关闭全员 AI 托管。" << std::endl;
                console::pause();
                continue;
        }
        displayBattle(); // HP/SP 变动后立即刷新显示
        console::pause();
        return false;
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

bool CombatSystem::processEnemyTurn(Combatant* enemy) {
    // 敌人 AI：眩晕跳过；否则按策略选择普攻或技能，集火我方残血
    if (enemy->hasStatusEffect(StatusEffect::Stun)) {
        addLog(enemy->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        aiPause();
        return false;
    }

    auto targets = getAliveAllies();
    if (targets.empty()) return false; // 无可攻击目标

    // 决策：有可用伤害技能且 SP 足够时，约 60% 概率用技能，否则普攻
    SkillBase* skill = chooseAISkill(enemy, targets, getAliveEnemies());
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
            return false;
        }
    }

    // 普通攻击：集火 HP 最低的我方
    Combatant* target = chooseAITarget(enemy, targets);
    if (!target) return false;
    displayBattle();
    performAttack(enemy, target, true);
    displayBattle();
    aiPause();
    return false;
}

// 我方 AI 托管回合：低血优先治疗，否则优先伤害技能，最后退回普攻
bool CombatSystem::processAllyAITurn(Combatant* actor) {
    // 眩晕跳过
    if (actor->hasStatusEffect(StatusEffect::Stun)) {
        addLog(actor->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        aiPause();
        return false;
    }

    auto allies = getAliveAllies();   // 含 actor 自身，治疗可选
    auto enemies = getAliveEnemies();
    if (enemies.empty()) return false;

    // 满血估算 = 100 + 等级×10；HP 低于 30% 视为残血
    int maxHpEst = 100 + actor->getLevel() * 10;
    bool lowHp = actor->getHP() < (maxHpEst * 3 / 10);

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
                return false;
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
            return false;
        }
    }

    // 3) 退回普攻：集火 HP 最低的敌方
    Combatant* t = chooseAITarget(actor, enemies);
    if (!t) return false;
    displayBattle();
    performAttack(actor, t, true);
    displayBattle();
    aiPause();
    return false;
}

// ---------------------------------------------------------------------------
// 行动执行
// ---------------------------------------------------------------------------

bool CombatSystem::performAttack(Combatant* attacker, Combatant* target, bool isNormalAttack) {
    // 命中判定（基于敏捷）
    float baseHit = isNormalAttack ? 0.95f : 0.90f;
    float hitRate = calculateHitRate(baseHit, attacker->getEffectiveStat(3), target->getEffectiveStat(3));
    if (roll(100) >= static_cast<int>(hitRate * 100)) {
        addLog(attacker->getName() + " 攻击 " + target->getName() + "，但未命中！");
        return true; // 未命中
    }

    // 伤害 = 力量 * 威力 - 防御（普通攻击威力固定为 10）
    int damage = calculateDamage(attacker->getEffectiveStat(0), 10, target->getEffectiveStat(2));
    target->takeDamage(damage);
    addLog(attacker->getName() + " 攻击 " + target->getName() + "，造成 " + std::to_string(damage) + " 点伤害。");
    return true;
}

bool CombatSystem::performSkill(Combatant* user, SkillBase* skill, std::vector<Combatant*>& targets) {
    // 检查并扣除 SP
    if (user->getSP() < skill->getCost()) {
        addLog(user->getName() + " 的 SP 不足，无法使用「" + skill->getName() + "」。");
        std::cout << user->getName() << " SP不足！" << std::endl;
        console::pause();
        return false;
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
        }
    } else if (auto* heal = dynamic_cast<HealSkill*>(skill)) {
        // 治疗技能
        for (auto* target : targets) {
            target->heal(heal->getHealAmount());
            addLog(user->getName() + " 对 " + target->getName() + " 使用「" + skill->getName() + "」，恢复 " + std::to_string(heal->getHealAmount()) + " 点 HP。");
        }
    } else {
        // 其他技能类型（如充能）：状态效果已移除（未实现）
        std::cout << "该技能类型：(未实现)" << std::endl;
        console::pause();
    }
    return true;
}

bool CombatSystem::performItem(Combatant* user, const std::string& itemId, std::vector<Combatant*>& targets) {
    // 道具系统：(未实现)
    std::cout << "道具系统：(未实现)" << std::endl;
    console::pause();
    return false;
}

bool CombatSystem::attemptRun(Combatant* runner) {
    if (config.disableRun) {
        addLog("此战斗禁止逃跑！");
        std::cout << "此战斗禁止逃跑！" << std::endl;
        console::pause();
        return false;
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
        return true;
    }

    addLog(runner->getName() + " 逃跑失败！");
    std::cout << runner->getName() << " 逃跑失败！" << std::endl;
    console::pause();
    return false;
}

// ---------------------------------------------------------------------------
// 辅助计算
// ---------------------------------------------------------------------------

int CombatSystem::roll(int upper) const {
    return std::uniform_int_distribution<int>(0, upper - 1)(rng);
}

float CombatSystem::calculateHitRate(float baseHitRate, int attackerAgility, int defenderAgility) {
    int sum = attackerAgility + defenderAgility;
    float agiFactor = (sum == 0) ? 0.5f : static_cast<float>(attackerAgility) / static_cast<float>(sum);
    float rate = baseHitRate * agiFactor + 0.05f;
    if (rate > 0.99f) rate = 0.99f;
    if (rate < 0.10f) rate = 0.10f;
    return rate;
}

int CombatSystem::calculateDamage(int strength, int power, int defense) {
    int dmg = strength * power - defense;
    if (dmg < 0) dmg = 0;
    return dmg;
}

std::vector<Combatant*> CombatSystem::getAliveEnemies() const {
    std::vector<Combatant*> res;
    for (auto* e : enemies) if (e->isAlive()) res.push_back(e);
    return res;
}

std::vector<Combatant*> CombatSystem::getAliveAllies() const {
    std::vector<Combatant*> res;
    if (player->isAlive()) res.push_back(player);
    for (auto* c : companions) {
        if (c->isAlive()) res.push_back(c);
    }
    return res;
}

// ---------------------------------------------------------------------------
// 状态效果 / AI（均未实现，仅保留空实现）
// ---------------------------------------------------------------------------

void CombatSystem::applyStatusEffects(Combatant* c) { /* 状态效果：(未实现) */ }
void CombatSystem::applyBurnDamage(Combatant* c) { /* 状态效果：(未实现) */ }
void CombatSystem::applySlowEffect(Combatant* c) { /* 状态效果：(未实现) */ }
void CombatSystem::checkStun(Combatant* c) { /* 状态效果：(未实现) */ }

SkillBase* CombatSystem::chooseAISkill(Combatant* ai, const std::vector<Combatant*>& enemies, const std::vector<Combatant*>& allies) {
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

const std::deque<std::string>& CombatSystem::getLog() const {
    return log.lines();
}
