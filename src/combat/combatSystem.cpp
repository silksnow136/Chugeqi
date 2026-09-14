#include "combatSystem.h"
#include "core/console.h"
#include <iostream>
#include <random>
#include <algorithm>

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------

CombatSystem::CombatSystem(Combatant* player, std::vector<Combatant*> companions,
                           std::vector<Combatant*> enemies,
                           const CombatConfig& config,
                           const ItemPool* itemPool)
    : player(player), companions(companions), enemies(enemies), config(config), itemPool(itemPool) {
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
    battleEnded = false;
    playerWon = false;
    playerEscaped = false;

    // 主循环：我方（玩家 + 同伴）→ 敌方。每轮统一在轮首结算、轮末递减状态。
    while (!battleEnded) {
        // 回合开始：灼烧扣血（眩晕的行动跳过在各回合函数内处理）
        applyRoundStartStatus();
        // 灼烧等持续伤害可能在轮首直接击杀，需在此即时判定胜负
        if (getAliveEnemies().empty()) { battleEnded = true; playerWon = true; break; }
        if (getAliveAllies().empty()) { battleEnded = true; playerWon = false; break; }

        // 玩家回合（手动或 AI 托管，内部会刷新界面）
        if (player->isAlive()) {
            processPlayerTurn();
        }
        // 逃跑成功会在行动内部把 battleEnded 置真，需立即结束，避免同伴/敌方再行动一轮
        if (battleEnded) break;
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

        // 回合结束：状态持续回合递减（对仍存活者）
        applyRoundEndStatus();
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
    if (playerEscaped) {
        addLog("成功逃跑，战斗结束。");
    } else {
        addLog(playerWon ? "战斗胜利！" : "战斗失败...");
    }
    displayBattle();
    if (playerEscaped) {
        std::cout << "成功逃跑，战斗结束。" << std::endl;
    } else {
        std::cout << (playerWon ? "战斗胜利！" : "战斗失败...") << std::endl;
    }
    console::pause();
    return playerWon;
}

// ---------------------------------------------------------------------------
// 日志
// ---------------------------------------------------------------------------

void CombatSystem::addLog(const std::string& msg) {
    log.add(msg);
}

const std::deque<std::string>& CombatSystem::getLog() const {
    return log.lines();
}

// ---------------------------------------------------------------------------
// 辅助计算
// ---------------------------------------------------------------------------

int CombatSystem::roll(int upper) const {
    return std::uniform_int_distribution<int>(0, upper - 1)(rng);
}

float CombatSystem::calculateHitRate(float baseHitRate, int attackerAgility, int defenderAgility) {
    // 命中率 = 基础命中 + 敏捷差加成（每点敏捷差 ±2%）。
    // 更快者命中更高、更慢者更低；上限 92%（最快攻击者对常规目标），下限 10%（保底）。
    float rate = baseHitRate + (attackerAgility - defenderAgility) * 0.02f;
    if (rate > 0.92f) rate = 0.92f;
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
// 状态效果结算
// ---------------------------------------------------------------------------

// 回合开始：对单个战斗者结算持续型状态效果。
// 灼烧在此扣血；眩晕的行动跳过在回合流程（manualTurn / processEnemyTurn / processAllyAITurn）中处理。
void CombatSystem::applyStatusEffects(Combatant* c) {
    if (!c->isAlive()) return;

    if (c->hasStatusEffect(StatusEffect::Burn)) {
        // 灼烧伤害 = 目标最大生命的 10%（至少 1 点）
        int damage = std::max(1, c->getMaxHP() / 10);
        c->takeDamage(damage);
        addLog(c->getName() + " 被灼烧，受到 " + std::to_string(damage) + " 点伤害。");
    }
}

void CombatSystem::applyRoundStartStatus() {
    for (auto* c : getAliveAllies()) applyStatusEffects(c);
    for (auto* e : getAliveEnemies()) applyStatusEffects(e);
}

void CombatSystem::applyRoundEndStatus() {
    for (auto* c : getAliveAllies()) c->updateStatusEffects();
    for (auto* e : getAliveEnemies()) e->updateStatusEffects();
}
