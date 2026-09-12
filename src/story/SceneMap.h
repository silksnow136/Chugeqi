#pragma once
// ---------------------------------------------------------------------------
// SceneMap — 将四幕地图（垓下/淮河/东城/乌江）接入主游戏的场景交互层
//
// 用 MapGrid 网格地图 + WASD 移动替换 map.cpp 中的简易 ASCII 地图。
// 走向友方 NPC → 触发对话（TalkManager）
// 走向药店    → 触发购买（PharManager）
// 走向铁匠铺  → 触发锻造（ForgeManager）
// 走向敌方    → 触发战斗（CombatSystem）
// 走向物品    → 拾取
// 按 ESC      → 退出场景地图，返回命令层
// ---------------------------------------------------------------------------

#include "MapGrid.h"

class Game;
class SceneManager;

namespace SceneMap {

// 根据场景 id / 分支构建对应的网格地图（仅布局，不含交互回调）
MapGrid buildSceneMap(int scene_id, int branch_id);

// 运行场景地图的 WASD 交互循环，回调接入游戏系统
// 返回 true 表示玩家正常退出（ESC），false 表示战斗失败等
bool runSceneMap(Game& game, SceneManager& sm, int scene_id, int branch_id);

} // namespace SceneMap
