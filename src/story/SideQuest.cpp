#include "SideQuest.h"
#include "Game.h"
#include "combat/character.h"
#include "core/console.h"
#include <iostream>

namespace SideQuest {

// ---------------------------------------------------------------------------
// 支线一：营地委托对话（老兵 / 粮官 / 粮仓）
// ---------------------------------------------------------------------------
static void talkQuest(Game& game, Combatant& player, QuestState& qs, const std::string& name) {
    console::clearScreen();
    console::setColor(10);
    std::cout << "\n===== " << name << " =====" << std::endl;
    console::setColor(7);

    if (name == "老兵") {
        if (qs.q1 == 0) {
            std::cout << "老兵：“大王！昨夜又有几名弟兄趁夜逃了……\n"
                         "        他们就在营外荒郊，求大王把他们劝回来！”\n";
            std::cout << "\n【接受委托】寻回逃兵（0/3）—— 从北门白色传送门进入荒郊\n";
            qs.q1 = 1;
        } else if (qs.q1 == 1) {
            std::cout << "老兵：“逃兵们就在营外荒郊，已劝回 " << qs.deserters << "/3。”\n";
        } else if (qs.q1 == 2) {
            std::cout << "老兵：“三名弟兄都回来了！多谢大王！\n";
            game.getGold() += 30;
            player.addItem("herb_potion", 1);
            qs.morale += 15;
            std::cout << "【奖励】金币 +30、伤药 ×1，军心 +15（当前军心 " << qs.morale << "）\n";
            qs.q1 = 3;
        } else {
            std::cout << "老兵：“军心已定，愿随大王死战！”\n";
        }
        console::pause();
        return;
    }

    if (name == "粮官") {
        if (qs.q2 == 0) {
            std::cout << "粮官：“南门最近有汉军哨骑出没，请大王巡视南门、将其击退！”\n";
            std::cout << "\n【接受委托】巡南门 —— 出南门击败汉军哨骑\n";
            qs.q2 = 1;
        } else if (qs.q2 == 1) {
            std::cout << "粮官：“南门外的汉军哨骑仍在游荡，请大王出手！”\n";
        } else if (qs.q2 == 2) {
            std::cout << "粮官：“哨骑已退！这是赏格，请大王收下。\n";
            game.getGold() += 20;
            player.addItem("iron_sword", 1);
            qs.morale += 10;
            std::cout << "【奖励】金币 +20、铁剑 ×1，军心 +10（当前军心 " << qs.morale << "）\n";
            qs.q2 = 3;
        } else {
            std::cout << "粮官：“南门安宁，全赖大王。”\n";
        }
        console::pause();
        return;
    }

    if (name == "粮仓") {
        if (qs.q3 == 0) {
            std::cout << "粮仓官：“仓中余粮无多，请大王示下如何分配？”\n\n"
                         "  1. 优先士兵 —— 伤药×2，军心 +5\n"
                         "  2. 优先战马 —— 草料×2，军心不变\n"
                         "  3. 留存突围 —— 木炭×2，军心 -5\n"
                         "\n请输入 1 / 2 / 3：";
            int key = console::readKey();
            if (key == '1') {
                player.addItem("herb_potion", 2);
                qs.morale += 5; qs.q3choice = 1;
                std::cout << "\n【分配】粮草优先士兵，营中士气大振。\n";
            } else if (key == '2') {
                player.addItem("fodder", 2);
                qs.q3choice = 2;
                std::cout << "\n【分配】粮草优先战马，马匹膘壮。\n";
            } else if (key == '3') {
                player.addItem("charcoal", 2);
                qs.morale -= 5; qs.q3choice = 3;
                std::cout << "\n【分配】粮草留存突围之用，士兵们略有微词。\n";
            } else {
                std::cout << "\n（未作分配）\n";
                console::pause();
                return;
            }
            std::cout << "当前军心 " << qs.morale << "\n";
            qs.q3 = 3;
        } else {
            std::cout << "粮仓官：“粮草已按大王的意思分派妥当。”\n";
        }
        console::pause();
    }
}

// 支线一：荒郊逃兵对话
static void talkDeserter(QuestState& qs, const std::string& name) {
    console::clearScreen();
    console::setColor(10);
    std::cout << "\n===== " << name << " =====" << std::endl;
    console::setColor(7);
    int idx = (name == "逃兵甲") ? 0 : (name == "逃兵乙" ? 1 : 2);
    if (!qs.deserterTalked[idx]) {
        qs.deserterTalked[idx] = true;
        qs.deserters++;
        if (idx == 0)      std::cout << "逃兵甲：“楚歌四起，弟兄们都以为家乡尽失……大王既亲来相劝，我跟您回去！”\n";
        else if (idx == 1) std::cout << "逃兵乙：“家中老母尚在……但大王不弃，我愿再持干戈！”\n";
        else               std::cout << "逃兵丙：“我等糊涂，听了一夜楚歌便散了军心——大王，走！”\n";
        std::cout << "\n已劝回逃兵 " << qs.deserters << "/3\n";
        if (qs.deserters >= 3 && qs.q1 == 1) {
            qs.q1 = 2;
            console::setColor(14);
            std::cout << "三名逃兵均已回心转意，回营地老兵处复命吧！\n";
            console::setColor(7);
        }
    } else {
        std::cout << "“大王，我这就随您回营。”\n";
    }
    console::pause();
}

// 支线二：田夫对话
static void talkTianfu(QuestState& qs) {
    console::clearScreen();
    console::setColor(10);
    std::cout << "\n===== 田夫 =====" << std::endl;
    console::setColor(7);
    if (!qs.yinlingUnlocked) {
        std::cout << "田夫：“将军不知，此去北岸有一条阴陵古道，\n"
                     "        迷雾深处连着古渡，只是从没人走通过……\n"
                     "        若要寻浅滩，需先有渡河图；寒水刺骨，还须蓑衣护体。”\n";
        qs.yinlingUnlocked = true;
        console::setColor(14);
        std::cout << "\n阴陵古道入口已在北岸显现（白色传送门 ↓）。\n";
        console::setColor(7);
    } else {
        std::cout << "田夫：“阴陵古道凶险，将军保重。雾中浅滩需凭渡河图辨认。”\n";
    }
    console::pause();
}

// ---------------------------------------------------------------------------
// 对外接口
// ---------------------------------------------------------------------------

bool tryHandleTalk(Game& game, Combatant& player, QuestState& qs,
                   const std::string& mapName, const std::string& npc) {
    // 支线一·营地委托
    if (mapName == "垓下营地" && (npc == "老兵" || npc == "粮官" || npc == "粮仓")) {
        talkQuest(game, player, qs, npc);
        return true;
    }
    // 支线一·荒郊逃兵
    if (mapName == "营外荒郊" && npc.compare(0, 2, "逃兵") == 0) {
        talkDeserter(qs, npc);
        return true;
    }
    // 支线二·田夫
    if (mapName == "淮河" && npc == "田夫") {
        talkTianfu(qs);
        return true;
    }
    return false;
}

void onBattleWon(QuestState& qs, const std::string& mapName, const std::string& npc) {
    // 委托二：南门外哨骑
    if (mapName == "垓下营地" && qs.q2 == 1 && npc == "汉军哨骑") {
        qs.q2 = 2;
        console::setColor(14);
        std::cout << "南门哨骑已肃清，回粮官处复命吧！\n";
        console::setColor(7);
        console::pause();
    }
    // 支线二：灌婴 BOSS
    if (mapName == "阴陵三" && npc == "灌婴") {
        qs.guanyingDefeated = true;
        console::setColor(14);
        std::cout << "灌婴败退！古渡返回之路已开启（上方白色传送门）。\n";
        console::setColor(7);
        console::pause();
    }
}

bool canEnterPortal(Combatant& player, QuestState& qs,
                    const std::string& mapName, const std::string& dest) {
    // 支线二：阴陵入口需田夫指路
    if (mapName == "淮河" && dest == "阴陵一" && !qs.yinlingUnlocked) {
        console::setColor(14);
        std::cout << "\n荒草丛生，似乎无路……先找附近的「田夫」问问吧。" << std::endl;
        console::setColor(7);
        console::pause();
        return false;
    }
    // 支线二：三层返回门需先击败灌婴
    if (mapName == "阴陵三" && dest == "淮河" && !qs.guanyingDefeated) {
        console::setColor(12);
        std::cout << "\n古渡被汉军封锁——先击败深处的灌婴！" << std::endl;
        console::setColor(7);
        console::pause();
        return false;
    }
    // 支线二：下三层时蓑衣检查（不阻止，仅首次提示扣血）
    if (mapName == "阴陵二" && dest == "阴陵三" &&
        player.getEquippedItemId(0) != "hr_raincoat" && !qs.raincoatWarned) {
        qs.raincoatWarned = true;
        player.takeDamage(10);
        console::setColor(12);
        std::cout << "\n寒水刺骨，项羽损失 10 点生命（装备蓑衣可免）。当前 HP "
                  << player.getHP() << std::endl;
        console::setColor(7);
        console::pause();
    }
    return true;
}

void onEnterMap(QuestState& qs, const std::string& mapName) {
    // 进入阴陵二层重置蓑衣提示
    if (mapName == "阴陵二") qs.raincoatWarned = false;
}

std::string advanceNarration(const QuestState& qs, int sceneId) {
    if (sceneId != 1) return "";
    switch (qs.q3choice) {
        case 1: return "营中士卒因多分粮草，突围时多有死战之士。";
        case 2: return "战马食饱，乌骓嘶鸣，行军快了几分。";
        case 3: return "留存的粮草支撑着这一路奔逃。";
        default: return "";
    }
}

} // namespace SideQuest
