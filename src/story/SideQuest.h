#pragma once
// ---------------------------------------------------------------------------
// SideQuest — 支线任务逻辑（与主线场景推进解耦）
//
// 只依赖 QuestState 状态 + Game/Combatant，不关心地图布局与渲染。
// runSceneMap 把交互事件转发到这里，主线只通过窄接口访问支线进度。
// ---------------------------------------------------------------------------
#include <string>
#include "QuestState.h"

class Game;
class Combatant;

namespace SideQuest {

// 支线对话：返回 true 表示该 NPC 由支线处理（否则由主线走 talk.json）
bool tryHandleTalk(Game& game, Combatant& player, QuestState& qs,
                   const std::string& mapName, const std::string& npc);

// 战斗胜利后更新支线进度（南门哨骑 / 灌婴 BOSS）
void onBattleWon(QuestState& qs, const std::string& mapName, const std::string& npc);

// 传送门前置检查：返回 false 阻止传送（田夫解锁 / 灌婴未败 / 蓑衣寒水）
bool canEnterPortal(Combatant& player, QuestState& qs,
                    const std::string& mapName, const std::string& dest);

// 进入子地图时的状态重置（阴陵二层重置蓑衣提示）
void onEnterMap(QuestState& qs, const std::string& mapName);

// 主线旁白里的支线插话（委托三的选择影响第一幕突围旁白），空串表示无插话
std::string advanceNarration(const QuestState& qs, int sceneId);

} // namespace SideQuest
