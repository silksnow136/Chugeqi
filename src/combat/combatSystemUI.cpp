#include "combatSystem.h"
#include "core/console.h"
#include <iostream>

// ---------------------------------------------------------------------------
// 界面显示
// ---------------------------------------------------------------------------

void CombatSystem::displayBattle() {
    console::clearScreen();
    console::setCursorVisible(false);  // 渲染期间隐藏光标

    // 每行补空格到固定宽度，覆盖旧内容残留
    const size_t WIDTH = 60;
    auto pad = [](const std::string& s, size_t w) {
        return s + std::string(s.size() < w ? w - s.size() : 0, ' ');
    };

    std::string out;

    // 战斗日志（固定 7 行）
    out += pad("=▽战斗日志▽============", WIDTH) + '\n';
    const auto& lines = log.lines();
    bool logEmpty = lines.empty();
    for (size_t i = 0; i < BattleLog::MAX_LINES; i++) {
        std::string line;
        if (i < lines.size()) {
            line = lines[i];
        } else if (i == BattleLog::MAX_LINES / 2 && logEmpty) {
            line = "                 (暂无)";
        }
        out += pad(line, WIDTH) + '\n';
    }

    // 我方状态（托管中时显示标记；同伴全灭则不显示同伴托管标记）
    bool hasAliveCompanion = false;
    for (auto* c : companions) { if (c->isAlive()) { hasAliveCompanion = true; break; } }
    std::string tag = playerAiAssisted ? "  [全员AI托管中，ESC退出]"
                                       : (companionAiAssisted && hasAliveCompanion ? "  [同伴AI托管中，ESC退出]" : "");
    out += pad("=▽我方▽================" + tag, WIDTH) + '\n';
    out += pad(displayStatus(player), WIDTH) + '\n';
    for (auto* companion : companions) {
        if (companion->isAlive()) out += pad(displayStatus(companion), WIDTH) + '\n';
    }

    // 敌方状态
    out += pad("=▽敌方▽================", WIDTH) + '\n';
    for (auto* e : getAliveEnemies()) {
        out += pad(displayStatus(e), WIDTH) + '\n';
    }
    out += pad("----------------------------------------", WIDTH) + '\n';

    std::cout << out;
    console::clearToEnd();  // 清掉角色死亡后减少的残留行
    console::setCursorVisible(true);
}

std::string CombatSystem::displayStatus(const Combatant* c) const {
    std::string s = c->getName() + "  Lv." + std::to_string(c->getLevel())
                  + "  HP:" + std::to_string(c->getHP()) + "/" + std::to_string(c->getMaxHP())
                  + "  SP:" + std::to_string(c->getSP()) + "/" + std::to_string(c->getMaxSP())
                  + "  状态：";
    std::vector<std::string> statuses;
    if (c->hasStatusEffect(StatusEffect::Burn))   statuses.push_back(statusName(StatusEffect::Burn));
    if (c->hasStatusEffect(StatusEffect::Slow))   statuses.push_back(statusName(StatusEffect::Slow));
    if (c->hasStatusEffect(StatusEffect::Stun))   statuses.push_back(statusName(StatusEffect::Stun));
    if (c->hasStatusEffect(StatusEffect::Charge)) statuses.push_back(statusName(StatusEffect::Charge));
    if (statuses.empty()) {
        s += "无";
    } else {
        for (size_t i = 0; i < statuses.size(); i++) {
            if (i) s += "/";
            s += statuses[i];
        }
    }
    return s;
}

// ---------------------------------------------------------------------------
// 菜单 / 目标选择 / 状态显示辅助
// ---------------------------------------------------------------------------

// 读取 [min, max] 范围内的数字键，返回对应的整数；
// allowBack 为 true 时，按 0 或 ESC 返回 -1（表示返回上一级菜单）。
int CombatSystem::readMenuChoice(int min, int max, bool allowBack) {
    while (true) {
        int key = console::readKey();
        if (allowBack && (key == 27 || key == '0')) return -1;
        if (key >= '0' + min && key <= '0' + max) {
            return key - '0';
        }
    }
}

// 显示技能列表并让玩家选择，返回选中的技能（无技能时返回 nullptr）
SkillBase* CombatSystem::selectSkill(Combatant* actor) {
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
Combatant* CombatSystem::selectTarget(const std::vector<Combatant*>& candidates, const std::string& prompt) {
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
std::vector<Combatant*> CombatSystem::buildSkillTargets(const std::vector<Combatant*>& allies,
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

// 状态效果中文名（战斗日志与界面显示共用，避免硬编码散落）
const char* CombatSystem::statusName(StatusEffect e) {
    switch (e) {
        case StatusEffect::Burn:   return "灼烧";
        case StatusEffect::Slow:   return "迟缓";
        case StatusEffect::Stun:   return "眩晕";
        case StatusEffect::Charge: return "充能";
        default:                   return "未知";
    }
}

// 伤害技能附带状态的默认持续回合数（skill.json 未配置持续回合时使用）
int CombatSystem::defaultStatusDuration(StatusEffect e) {
    switch (e) {
        case StatusEffect::Stun: return 1; // 眩晕仅持续一回合
        default:                 return 3;
    }
}
