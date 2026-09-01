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

        // 同伴回合（手动操控）
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

    // 我方状态
    std::cout << "=▽我方▽================" << std::endl;
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

bool CombatSystem::processPlayerTurn() { return manualTurn(player, 4); }

bool CombatSystem::processCompanionTurn(Combatant* companion) { return manualTurn(companion, 2); }

// 通用手动回合：玩家与同伴共用。maxChoice 决定菜单项数（玩家 4，同伴 2）
bool CombatSystem::manualTurn(Combatant* actor, int maxChoice) {
    while (true) {
        displayBattle(); // 行动前刷新一次界面
        if (maxChoice == 4)
            std::cout << "[1]攻击  [2]技能  [3]道具(未实现)  [4]逃跑" << std::endl;
        else
            std::cout << actor->getName() << " 的行动：[1]攻击  [2]技能" << std::endl;

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
            case 3: // 道具（未实现）
                std::cout << "道具系统：(未实现)" << std::endl;
                console::pause();
                continue;
            case 4: // 逃跑（仅玩家）
                return attemptRun(actor);
        }
        displayBattle(); // HP/SP 变动后立即刷新显示
        console::pause();
        return false;
    }
}

bool CombatSystem::processEnemyTurn(Combatant* enemy) {
    // 敌人 AI：眩晕跳过；否则按策略选择普攻或技能，集火我方残血
    if (enemy->hasStatusEffect(StatusEffect::Stun)) {
        addLog(enemy->getName() + " 处于眩晕，无法行动！");
        displayBattle();
        console::pause();
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
            console::pause();
            return false;
        }
    }

    // 普通攻击：集火 HP 最低的我方
    Combatant* target = chooseAITarget(enemy, targets);
    if (!target) return false;
    displayBattle();
    performAttack(enemy, target, true);
    displayBattle();
    console::pause();
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
