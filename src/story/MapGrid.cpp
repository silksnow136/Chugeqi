#include "MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>

// 每格固定显示宽度（控制台列数）：2 个中文字符 = 4 列
static const int CELL_WIDTH = 4;

// 空格填充和墙壁显示
static const std::string EMPTY_DISP = "    ";   // 4 个空格
static const std::string WALL_DISP  = "████";   // 4 个全块字符，终端中占 4 列

// 计算 UTF-8 字符串的控制台显示宽度（CJK 字符 = 2，ASCII = 1）
static int displayWidth(const std::string& s) {
    int width = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80)        { width += 1; i += 1; }  // ASCII
        else if (c < 0xC0)   { i += 1; }               // 续字节，跳过
        else if (c < 0xE0)   { width += 1; i += 2; }  // 2 字节（西文重音等）
        else if (c < 0xF0)   {                         // 3 字节
            i += 3;
            // 提取码点判断：U+2500-U+257F（制表/方块元素）在 Windows 终端按 1 列
            unsigned cp = ((c & 0x0F) << 12) |
                          ((static_cast<unsigned char>(s[i-2]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(s[i-1]) & 0x3F);
            width += (cp >= 0x2500 && cp <= 0x257F) ? 1 : 2;
        }
        else                 { width += 2; i += 4; }  // 4 字节（emoji）
    }
    return width;
}

// 将字符串补空格到固定列宽
static std::string padToWidth(const std::string& s, int targetWidth) {
    int dw = displayWidth(s);
    if (dw >= targetWidth) return s;
    return s + std::string(targetWidth - dw, ' ');
}

MapGrid::MapGrid(int rows, int cols) {
    // 最小尺寸保护
    if (rows < 3) rows = 3;
    if (cols < 3) cols = 3;

    grid.resize(rows, std::vector<Tile>(cols));

    // 四周边框设为墙壁
    for (int r = 0; r < rows; r++) {
        grid[r][0] = Tile(WALL_DISP, TileType::WALL);
        grid[r][cols - 1] = Tile(WALL_DISP, TileType::WALL);
    }
    for (int c = 0; c < cols; c++) {
        grid[0][c] = Tile(WALL_DISP, TileType::WALL);
        grid[rows - 1][c] = Tile(WALL_DISP, TileType::WALL);
    }

    // 内部默认空地
    for (int r = 1; r < rows - 1; r++) {
        for (int c = 1; c < cols - 1; c++) {
            grid[r][c] = Tile(EMPTY_DISP, TileType::EMPTY);
        }
    }
}

void MapGrid::setTile(int row, int col, const std::string& display, TileType type, const std::string& name) {
    if (!isValid(row, col)) return;
    // 不覆盖玩家位置
    if (row == playerRow && col == playerCol) return;
    grid[row][col] = Tile(display, type, name);
}

void MapGrid::setPlayer(int row, int col) {
    if (!isValid(row, col)) return;
    // 清除旧位置的玩家标记
    grid[playerRow][playerCol] = Tile(EMPTY_DISP, TileType::EMPTY);
    playerRow = row;
    playerCol = col;
    grid[row][col] = Tile("项羽", TileType::PLAYER, "项羽");
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
            std::cout << padToWidth(tile.display, CELL_WIDTH);
        }
        std::cout << std::endl;
    }

    console::setColor(14);
    std::cout << "-------------------------------" << std::endl;
    console::setColor(7);
    std::cout << "WASD: 移动  |  项羽=玩家  友方(绿)→对话  敌方(红)→战斗  药店(青)→购买  铁匠(紫)→锻造  █=墙" << std::endl;
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
        grid[playerRow][playerCol] = Tile(EMPTY_DISP, TileType::EMPTY);
        // 新位置放玩家
        playerRow = newRow;
        playerCol = newCol;
        grid[newRow][newCol] = Tile("项羽", TileType::PLAYER, "项羽");
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
