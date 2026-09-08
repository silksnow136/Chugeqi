#include "MapGrid.h"
#include "core/console.h"
#include <iostream>
#include <cctype>

// 每格固定显示宽度（控制台列数）：2 个中文字符 = 4 列
static const int CELL_WIDTH = 4;

// 空格填充和墙壁显示
static const std::string EMPTY_DISP = "    ";   // 4 个空格
static const std::string WALL_DISP  = "████";   // 4 个全块字符，终端中占 4 列

// 拟物装饰字符（各场景复用）
static const std::string ACCOUNT = "====";   // 帐墙（帐篷布横纹）
static const std::string FENCE   = "║║║║";   // 栅栏（木条）
static const std::string CHEVAL  = "╳╳╳╳";   // 拒马（交叉木）
static const std::string WATER   = "~~~~";   // 水

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

void MapGrid::setTile(int row, int col, const std::string& display, TileType type, const std::string& name, int color) {
    if (!isValid(row, col)) return;
    // 不覆盖玩家位置
    if (row == playerRow && col == playerCol) return;
    grid[row][col] = Tile(display, type, name, color);
}

void MapGrid::setPlayer(int row, int col) {
    if (!isValid(row, col)) return;
    // 清除旧位置的玩家标记
    grid[playerRow][playerCol] = Tile(EMPTY_DISP, TileType::EMPTY);
    playerRow = row;
    playerCol = col;
    grid[row][col] = Tile("项羽", TileType::PLAYER, "项羽");
}

// 帐墙房间：四边墙 + 南墙留门
void MapGrid::buildRoom(int r1, int c1, int r2, int c2, int doorCol) {
    for (int c = c1; c <= c2; c++) setTile(r1, c, ACCOUNT, TileType::WALL, "", 7);
    for (int c = c1; c <= c2; c++) if (c != doorCol) setTile(r2, c, ACCOUNT, TileType::WALL, "", 7);
    for (int r = r1; r <= r2; r++) setTile(r, c1, ACCOUNT, TileType::WALL, "", 7);
    for (int r = r1; r <= r2; r++) setTile(r, c2, ACCOUNT, TileType::WALL, "", 7);
}

// 栅栏壁垒：横向墙 + 中间留门
void MapGrid::buildFence(int row, int c1, int c2, int doorC1, int doorC2) {
    for (int c = c1; c <= c2; c++) if (c < doorC1 || c > doorC2) setTile(row, c, FENCE, TileType::WALL, "", 6);
}

// 横向水沟
void MapGrid::buildWater(int row, int c1, int c2) {
    for (int c = c1; c <= c2; c++) setTile(row, c, WATER, TileType::WALL, "", 1);
}

// 单个拒马
void MapGrid::buildCheval(int row, int col) {
    setTile(row, col, CHEVAL, TileType::WALL, "", 8);
}

void MapGrid::render() const {
    // 首次清屏，之后光标回顶覆盖重绘（避免清屏闪烁）
    static bool firstRender = true;
    if (firstRender) {
        console::clearScreen();
        firstRender = false;
    } else {
        console::moveCursor(0, 0);
    }
    console::setCursorVisible(false);  // 渲染期间隐藏光标，避免光标乱闪
    console::setColor(15);
    std::cout << "========== 场景地图 ==========" << std::endl;
    console::setColor(7);

    for (const auto& row : grid) {
        for (const auto& tile : row) {
            // 根据类型取默认色（低饱和深色系），若格内指定了颜色则覆盖
            int color = 7;
            switch (tile.type) {
                case TileType::WALL:      color = 8;  break;  // 灰（边框）
                case TileType::PLAYER:    color = 6;  break;  // 棕（项羽）
                case TileType::FRIEND:    color = 2;  break;  // 深绿
                case TileType::ENEMY:     color = 4;  break;  // 深红
                case TileType::PHARMACY:  color = 3;  break;  // 深青
                case TileType::FORGE:     color = 5;  break;  // 深紫
                case TileType::ITEM:      color = 14; break;  // 黄（拾取亮点）
                default:                  color = 7;  break;
            }
            if (tile.color >= 0) color = tile.color;
            console::setColor(color);
            std::cout << padToWidth(tile.display, CELL_WIDTH);
        }
        std::cout << std::endl;
    }

    console::setColor(15);
    std::cout << "-------------------------------" << std::endl;
    console::setColor(7);
    std::cout << "WASD: 移动  |  项羽=玩家  友方(绿)→对话  敌方(红)→战斗  物品(黄)→拾取  药店(青)→购买  铁匠(紫)→锻造  █=墙" << std::endl;
    std::cout << "> ";
    console::setCursorVisible(true);  // 渲染完恢复光标（停在提示符处）
    console::clearToEnd();  // 清掉 "> " 之后的对话残留（底部提示动态附着）
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

    // 物品：拾取后消失，玩家移动过去
    if (target.type == TileType::ITEM) {
        interactionType = TileType::ITEM;
        interactionName = target.name;
        triggerInteraction(TileType::ITEM, target.name);
        grid[playerRow][playerCol] = Tile(EMPTY_DISP, TileType::EMPTY);
        playerRow = newRow;
        playerCol = newCol;
        grid[newRow][newCol] = Tile("项羽", TileType::PLAYER, "项羽");
        return true;  // 触发了拾取
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
        case TileType::ITEM:
            if (onItem) onItem(name);
            break;
        default:
            break;
    }
}
