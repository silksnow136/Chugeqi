#include "MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>

MapGrid::MapGrid(int rows, int cols) {
    // 最小尺寸保护
    if (rows < 3) rows = 3;
    if (cols < 3) cols = 3;

    grid.resize(rows, std::vector<Tile>(cols));

    // 四周边框设为墙壁
    for (int r = 0; r < rows; r++) {
        grid[r][0] = Tile('#', TileType::WALL);
        grid[r][cols - 1] = Tile('#', TileType::WALL);
    }
    for (int c = 0; c < cols; c++) {
        grid[0][c] = Tile('#', TileType::WALL);
        grid[rows - 1][c] = Tile('#', TileType::WALL);
    }

    // 内部默认空地
    for (int r = 1; r < rows - 1; r++) {
        for (int c = 1; c < cols - 1; c++) {
            grid[r][c] = Tile('.', TileType::EMPTY);
        }
    }
}

void MapGrid::setTile(int row, int col, char display, TileType type, const std::string& name) {
    if (!isValid(row, col)) return;
    // 不覆盖玩家位置
    if (row == playerRow && col == playerCol) return;
    grid[row][col] = Tile(display, type, name);
}

void MapGrid::setPlayer(int row, int col) {
    if (!isValid(row, col)) return;
    // 清除旧位置的玩家标记
    grid[playerRow][playerCol] = Tile('.', TileType::EMPTY);
    playerRow = row;
    playerCol = col;
    grid[row][col] = Tile('@', TileType::PLAYER, "项羽");
}

void MapGrid::render() const {
    console::clearScreen();
    console::setColor(14);
    std::cout << "========== 场景地图 ==========" << std::endl;
    console::setColor(7);

    for (const auto& row : grid) {
        for (const auto& tile : row) {
            // 根据类型着色
            switch (tile.type) {
                case TileType::WALL:
                    console::setColor(8);  // 暗灰
                    break;
                case TileType::PLAYER:
                    console::setColor(12); // 亮红
                    break;
                case TileType::FRIEND:
                    console::setColor(10); // 亮绿
                    break;
                case TileType::ENEMY:
                    console::setColor(12); // 亮红
                    break;
                case TileType::PHARMACY:
                    console::setColor(11); // 亮青
                    break;
                case TileType::FORGE:
                    console::setColor(13); // 亮紫
                    break;
                default:
                    console::setColor(7);  // 灰白
                    break;
            }
            std::cout << tile.display;
        }
        std::cout << std::endl;
    }

    console::setColor(14);
    std::cout << "-------------------------------" << std::endl;
    console::setColor(7);
    std::cout << "WASD: 移动项羽  |  @=项羽 F=友方 E=敌方 P=药店 S=铁匠 #=墙 .=空地" << std::endl;
    std::cout << "> ";
}

bool MapGrid::move(char direction) {
    int newRow = playerRow;
    int newCol = playerCol;

    switch (std::tolower(direction)) {
        case 'w': newRow--; break;  // 上
        case 'a': newCol--; break;  // 左
        case 's': newRow++; break;  // 下
        case 'd': newCol++; break;  // 右
        default: return false;      // 非法按键
    }

    // 边界检测：越界不动
    if (!isValid(newRow, newCol)) return false;

    Tile& target = grid[newRow][newCol];

    // 墙壁：不能通过
    if (target.type == TileType::WALL) return false;

    // 空地：移动玩家
    if (target.type == TileType::EMPTY) {
        // 旧位置恢复空地
        grid[playerRow][playerCol] = Tile('.', TileType::EMPTY);
        // 新位置放玩家
        playerRow = newRow;
        playerCol = newCol;
        grid[newRow][newCol] = Tile('@', TileType::PLAYER, "项羽");
        return false;  // 未触发交互
    }

    // 其他单位：不移动，触发交互
    interactionType = target.type;
    interactionName = target.name;
    triggerInteraction(target.type, target.name);
    return true;  // 触发了交互
}

bool MapGrid::isValid(int row, int col) const {
    return row >= 0 && row < static_cast<int>(grid.size()) &&
           col >= 0 && col < static_cast<int>(grid[0].size());
}

void MapGrid::triggerInteraction(TileType type, const std::string& name) {
    switch (type) {
        case TileType::FRIEND:
            if (onTalk) onTalk(name);
            break;
        case TileType::ENEMY:
            if (onBattle) onBattle(name);
            break;
        case TileType::PHARMACY:
            if (onPharmacy) onPharmacy(name);
            break;
        case TileType::FORGE:
            if (onForge) onForge(name);
            break;
        default:
            break;
    }
}
