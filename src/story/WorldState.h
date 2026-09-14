#pragma once
#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// WorldState — 地图持久状态（跨传送门 / 读档保留）
//   记录：当前所在子地图、玩家坐标、每张地图上已被清除的格子
//   （击败的敌人、已拾取的道具），避免往返地图时状态被刷新。
// ---------------------------------------------------------------------------
struct WorldState {
    // 当前所在子地图名；空串表示不在地图内（主菜单 / 剧情播放中）
    std::string mapName;

    // 玩家在当前地图的坐标；不在地图内时为 -1
    int playerRow = -1;
    int playerCol = -1;

    // 每张地图已被清除的格子，坐标为 row*1000 + col（地图尺寸远小于 1000）
    std::map<std::string, std::vector<int>> cleared;

    // 标记某地图某格已清除（击败敌人 / 拾取道具）；重复标记自动忽略
    void markCleared(const std::string& map, int row, int col) {
        int code = row * 1000 + col;
        auto& v = cleared[map];
        for (int c : v) if (c == code) return;
        v.push_back(code);
    }
};
