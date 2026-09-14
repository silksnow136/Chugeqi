#pragma once
// ---------------------------------------------------------------------------
// MapLayouts — 四幕主线地图 + 支线子地图的静态布局定义（只负责"摆格子"，不含交互）
// ---------------------------------------------------------------------------
#include "MapGrid.h"
#include "combat/character.h"

namespace MapLayouts {

// 根据场景 id 构建对应幕的主地图（仅布局，不含交互回调）
MapGrid buildSceneMap(int scene_id, int branch_id);

// 按地图名构建子地图（含进入时的落点）
MapGrid buildNamedMap(const std::string& name, const Combatant& player);

} // namespace MapLayouts
