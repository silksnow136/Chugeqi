#include "MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>

// 每格固定显示宽度（控制台列数）
static const int CELL_WIDTH = 4;

static const std::string EMPTY_DISP = "    ";     // 4 空格
static const std::string WALL_DISP  = "████";     // 4 个全块
static const std::string ROOM_WALL  = "====";     // 帐墙
static const std::string FENCE_DISP = "║║";       // 栅栏（2 列 + 2 空格对齐）
static const std::string CHEVAL_DISP = "╳╳ ";      // 拒马
static const std::string WATER_DISP = "~~~~";      // 水域

// 计算 UTF-8 字符串的控制台显示宽度
static int displayWidth(const std::string& s) {
    int width = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80)        { width += 1; i += 1; }
        else if (c < 0xC0)   { i += 1; }
        else if (c < 0xE0)   { width += 1; i += 2; }
        else if (c < 0xF0)   {
            i += 3;
            unsigned cp = ((c & 0x0F) << 12) |
                          ((static_cast<unsigned char>(s[i-2]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(s[i-1]) & 0x3F);
            width += (cp >= 0x2500 && cp <= 0x257F) ? 1 : 2;
        }
        else                 { width += 2; i += 4; }
    }
    return width;
}

static std::string padToWidth(const std::string& s, int targetWidth) {
    int dw = displayWidth(s);
    if (dw >= targetWidth) return s;
    return s + std::string(targetWidth - dw, ' ');
}

// =========================================================================
// 构造
// =========================================================================

MapGrid::MapGrid(int rows, int cols) {
    if (rows < 3) rows = 3;
    if (cols < 3) cols = 3;

    grid.resize(rows, std::vector<Tile>(cols));

    // 四周边框
    for (int r = 0; r < rows; r++) {
        grid[r][0] = Tile(WALL_DISP, TileType::WALL);
        grid[r][cols - 1] = Tile(WALL_DISP, TileType::WALL);
    }
    for (int c = 0; c < cols; c++) {
        grid[0][c] = Tile(WALL_DISP, TileType::WALL);
        grid[rows - 1][c] = Tile(WALL_DISP, TileType::WALL);
    }

    // 内部空地
    for (int r = 1; r < rows - 1; r++)
        for (int c = 1; c < cols - 1; c++)
            grid[r][c] = Tile(EMPTY_DISP, TileType::EMPTY);
}

// =========================================================================
// 建筑方法
// =========================================================================

void MapGrid::buildRoom(int r1, int c1, int r2, int c2, int doorCol) {
    // 上下墙
    for (int c = c1; c <= c2; c++) {
        if (c != doorCol && isValid(r1, c))
            grid[r1][c] = Tile(ROOM_WALL, TileType::WALL);
        if (c != doorCol && isValid(r2, c))
            grid[r2][c] = Tile(ROOM_WALL, TileType::WALL);
    }
    // 左右墙
    for (int r = r1; r <= r2; r++) {
        if (isValid(r, c1))
            grid[r][c1] = Tile(ROOM_WALL, TileType::WALL);
        if (isValid(r, c2))
            grid[r][c2] = Tile(ROOM_WALL, TileType::WALL);
    }
    // 门：放在上下墙的 doorCol 位置，设为 DOOR 类型
    if (isValid(r1, doorCol))
        grid[r1][doorCol] = Tile("门", TileType::DOOR, "门");
    if (isValid(r2, doorCol))
        grid[r2][doorCol] = Tile("门", TileType::DOOR, "门");

    // 内部清空
    for (int r = r1 + 1; r < r2; r++)
        for (int c = c1 + 1; c < c2; c++)
            if (isValid(r, c) && grid[r][c].type == TileType::WALL && grid[r][c].display == ROOM_WALL)
                grid[r][c] = Tile(EMPTY_DISP, TileType::EMPTY);
}

void MapGrid::buildFence(int r, int c, int length, int doorCol, int doorCol2) {
    // 横向栅栏：在 row=r 行，从 col=c 开始向右延伸 length 格
    for (int i = 0; i < length; i++) {
        int cc = c + i;
        if (!isValid(r, cc)) break;
        if (grid[r][cc].type == TileType::PLAYER) continue;
        // 门位置设为 DOOR
        if (cc == doorCol || cc == doorCol2) {
            grid[r][cc] = Tile("门", TileType::DOOR, "门");
        } else {
            grid[r][cc] = Tile(FENCE_DISP, TileType::WALL);
        }
    }
}

void MapGrid::buildCheval(int r, int c) {
    if (!isValid(r, c)) return;
    if (grid[r][c].type == TileType::PLAYER) return;
    grid[r][c] = Tile(CHEVAL_DISP, TileType::WALL);
}

void MapGrid::buildWater(int r, int c, int length) {
    for (int i = 0; i < length; i++) {
        int cc = c + i;
        if (!isValid(r, cc)) break;
        if (grid[r][cc].type == TileType::PLAYER) continue;
        grid[r][cc] = Tile(WATER_DISP, TileType::WALL);
    }
}

// =========================================================================
// 设置
// =========================================================================

void MapGrid::setTile(int row, int col, const std::string& display, TileType type, const std::string& name) {
    if (!isValid(row, col)) return;
    if (row == playerRow && col == playerCol) return;
    grid[row][col] = Tile(display, type, name);
}

void MapGrid::setPlayer(int row, int col) {
    if (!isValid(row, col)) return;
    // 清除旧位置（恢复 underPlayer）
    if (playerRow >= 1 && playerCol >= 1 && isValid(playerRow, playerCol)) {
        grid[playerRow][playerCol] = underPlayer;
    }
    // 保存新位置原来的格子
    underPlayer = grid[row][col];
    playerRow = row;
    playerCol = col;
    grid[row][col] = Tile("项羽", TileType::PLAYER, "项羽");
}

// =========================================================================
// 渲染 — 用光标定位覆盖重绘，避免 cls 导致的闪烁/重复
// =========================================================================

void MapGrid::render() const {
    // 首屏用 cls 彻底清屏，后续用光标定位覆盖避免闪烁/重复
    if (firstRender) {
        console::clearScreen();
        firstRender = false;
    }
    console::setCursorVisible(false);
    console::moveCursor(0, 0);

    // 标题
    console::setColor(14);
    std::cout << "========== 场景地图 ==========" << std::endl;
    console::setColor(7);

    // 地图各行
    for (const auto& row : grid) {
        for (const auto& tile : row) {
            switch (tile.type) {
                case TileType::WALL:
                    console::setColor(8);  break;
                case TileType::PLAYER:
                    console::setColor(12); break;
                case TileType::FRIEND:
                    console::setColor(10); break;
                case TileType::ENEMY:
                    console::setColor(12); break;
                case TileType::PHARMACY:
                    console::setColor(11); break;
                case TileType::FORGE:
                    console::setColor(13); break;
                case TileType::DOOR:
                    console::setColor(14); break;
                case TileType::ITEM:
                    console::setColor(14); break;
                default:
                    console::setColor(7);  break;
            }
            std::cout << padToWidth(tile.display, CELL_WIDTH);
        }
        std::cout << "  \n"; // 行尾补空格防残留
    }

    console::setColor(14);
    std::cout << "-------------------------------" << std::endl;
    console::setColor(7);
    std::cout << "WASD: 移动  |  项羽=玩家  友方(绿)→对话  敌方(红)→战斗  药店(青)→购买  铁匠(紫)→锻造  门(黄)→通行  █=墙" << std::endl;
    std::cout << "> ";

    console::setCursorVisible(true);
    console::clearToEnd();  // 清掉交互后残留的对话文字
}

// =========================================================================
// 移动逻辑
// =========================================================================

bool MapGrid::move(char direction) {
    int newRow = playerRow;
    int newCol = playerCol;

    switch (std::tolower(direction)) {
        case 'w': newRow--; break;
        case 'a': newCol--; break;
        case 's': newRow++; break;
        case 'd': newCol++; break;
        default: return false;
    }

    if (!isValid(newRow, newCol)) return false;

    Tile& target = grid[newRow][newCol];

    // 墙壁：不能通过
    if (target.type == TileType::WALL) return false;

    // 空地或门：可通行
    if (target.type == TileType::EMPTY || target.type == TileType::DOOR) {
        // 恢复旧位置为 underPlayer（门会保留，空地恢复空格）
        grid[playerRow][playerCol] = underPlayer;
        // 保存新位置原来的格子
        underPlayer = target;
        // 移动玩家
        playerRow = newRow;
        playerCol = newCol;
        grid[newRow][newCol] = Tile("项羽", TileType::PLAYER, "项羽");
        return false;
    }

    // 物品：拾取后消失，玩家移动到该格
    if (target.type == TileType::ITEM) {
        interactionType = target.type;
        interactionName = target.name;
        if (onItem) onItem(target.name);
        // 物品消失 → 变空地
        grid[newRow][newCol] = Tile(EMPTY_DISP, TileType::EMPTY);
        // 恢复旧位置
        grid[playerRow][playerCol] = underPlayer;
        underPlayer = Tile(EMPTY_DISP, TileType::EMPTY);
        // 移动玩家
        playerRow = newRow;
        playerCol = newCol;
        grid[newRow][newCol] = Tile("项羽", TileType::PLAYER, "项羽");
        return true;
    }
    interactionType = target.type;
    interactionName = target.name;
    triggerInteraction(target.type, target.name);
    return true;
}

bool MapGrid::isValid(int row, int col) const {
    return row >= 0 && row < static_cast<int>(grid.size()) &&
           col >= 0 && col < static_cast<int>(grid[0].size());
}

void MapGrid::triggerInteraction(TileType type, const std::string& name) {
    switch (type) {
        case TileType::FRIEND:  if (onTalk) onTalk(name); break;
        case TileType::ENEMY:   if (onBattle) onBattle(name); break;
        case TileType::PHARMACY: if (onPharmacy) onPharmacy(name); break;
        case TileType::FORGE:   if (onForge) onForge(name); break;
        case TileType::ITEM:    if (onItem) onItem(name); break;
        default: break;
    }
}
