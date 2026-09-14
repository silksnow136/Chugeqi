// ---------------------------------------------------------------------------
// portalTestMain.cpp — 传送门地图链测试（二维网格）
//
// 6 张小型空地图排成 3 列 × 2 行，相邻地图用成对传送门连通（上下左右）。
// WASD 移动项羽，走向传送门传送到相邻地图，ESC 退出。
// ---------------------------------------------------------------------------

#include "story/MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>
#include <functional>

static const int GRID_COLS = 3;
static const int GRID_ROWS = 2;
static const int MAP_COUNT = GRID_COLS * GRID_ROWS;
static const int MAP_ROWS = 15;
static const int MAP_COLS = 17;

static const char* kMapNames[MAP_COUNT] = { "图一", "图二", "图三", "图四", "图五", "图六" };

static int mapIndexByName(const std::string& name) {
    for (int i = 0; i < MAP_COUNT; i++)
        if (name == kMapNames[i]) return i;
    return -1;
}

static MapGrid buildMap(int index) {
    MapGrid grid(MAP_ROWS, MAP_COLS);
    int r = index / GRID_COLS;
    int c = index % GRID_COLS;
    grid.setPlayer(MAP_ROWS / 2, MAP_COLS / 2);

    // 顶部地图名
    grid.setTile(1, MAP_COLS / 2, kMapNames[index], TileType::FRIEND, kMapNames[index]);

    // 四个方向传送门（连通相邻地图）
    if (r > 0)
        grid.buildPortal(2, MAP_COLS / 2, PortalDir::Up, kMapNames[index - GRID_COLS], 0);
    if (r < GRID_ROWS - 1)
        grid.buildPortal(MAP_ROWS - 3, MAP_COLS / 2, PortalDir::Down, kMapNames[index + GRID_COLS], 0);
    if (c > 0)
        grid.buildPortal(MAP_ROWS / 2, 2, PortalDir::Left, kMapNames[index - 1], 0);
    if (c < GRID_COLS - 1)
        grid.buildPortal(MAP_ROWS / 2, MAP_COLS - 3, PortalDir::Right, kMapNames[index + 1], 0);

    return grid;
}

int main() {
    console::init();

    int cur = 0;
    MapGrid grid = buildMap(cur);

    std::function<void(TileType, const std::string&)> interact;
    interact = [&](TileType type, const std::string& name) {
        if (type == TileType::FRIEND) {
            console::setColor(10);
            std::cout << "\n当前位于「" << name << "」。" << std::endl;
            console::setColor(7);
            console::pause();
        } else if (type == TileType::PORTAL) {
            int target = mapIndexByName(name);
            if (target < 0) return;
            cur = target;
            grid = buildMap(cur);
            grid.onInteract = interact;
            grid.render();
        }
    };
    grid.onInteract = interact;

    grid.render();
    std::cout << "传送门测试 —— WASD 移动，走向传送门传送到相邻地图，ESC 退出" << std::endl;

    while (true) {
        int key = console::readKey();
        if (key == 27) break;
        char dir = static_cast<char>(std::tolower(key));
        if (dir == 'w' || dir == 'a' || dir == 's' || dir == 'd') {
            grid.move(dir);
            grid.render();
        }
    }

    console::clearScreen();
    console::setColor(14);
    std::cout << "已退出。" << std::endl;
    console::setColor(7);
    return 0;
}
