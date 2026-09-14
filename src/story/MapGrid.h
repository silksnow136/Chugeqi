#pragma once
// ---------------------------------------------------------------------------
// 网格地图系统：WASD 控制项羽移动，边界检测，碰撞触发交互
//
// 设计：
//   - 二维网格，每格为一个 Tile（显示字符串 + 类型 + 名称）
//   - 每格固定占 4 个控制台列宽（= 2 个中文字符），不足补空格
//   - 玩家（项羽）用 WASD 移动，每次移动一格
//   - 不能越过边界墙，不能覆盖其他文字单位
//   - 试图覆盖单位时触发交互：友方→对话，敌方→战斗，药店→购买，物品→拾取
//   - 门(DOOR)可通行，玩家走过后恢复原样
// ---------------------------------------------------------------------------

#include <vector>
#include <string>
#include <functional>

enum class TileType {
    EMPTY,      // 可通行空地（空格）
    WALL,       // 边界/墙壁（█）
    PLAYER,     // 项羽（玩家）
    FRIEND,     // 友方单位 → 触发对话
    ENEMY,      // 敌方单位 → 触发战斗
    PHARMACY,   // 药店 → 触发购买
    DOOR,       // 门 → 可通行，走过后恢复
    ITEM,       // 物品 → 拾取后消失
    ADVANCE,    // 跳转点 → 触发幕次跳转（帅帐/渡口等）
    PORTAL      // 传送门 → 连接地图，触发地图切换
};

// 传送门方向：决定箭头指向（←/↑/→/↓）与条延伸方向（条统一横向）
enum class PortalDir { Left, Up, Right, Down };

struct Tile {
    std::string display;    // 显示字符串
    TileType type = TileType::EMPTY;
    std::string name;       // 单位名称 / 传送目标地图名
    int portalDir = -1;     // 传送门方向（PortalDir 枚举值），非传送门为 -1

    Tile() = default;
    Tile(const std::string& d, TileType t, const std::string& n = "", int dir = -1)
        : display(d), type(t), name(n), portalDir(dir) {}
};

class MapGrid {
public:
    // 构造：指定网格大小，自动用墙壁围边框
    MapGrid(int rows, int cols);

    // 设置某格内容（不覆盖玩家位置）
    void setTile(int row, int col, const std::string& display, TileType type, const std::string& name = "");

    // 清除某格内容，恢复为空地（击败敌人后使其从地图上消失）
    void clearTile(int row, int col);

    // 设置玩家初始位置
    void setPlayer(int row, int col);

    // 渲染整个地图到控制台（使用光标定位避免重绘闪烁/重复）
    void render() const;

    // WASD 移动
    void move(char direction);

    int getPlayerRow() const { return playerRow; }
    int getPlayerCol() const { return playerCol; }

    // ===== 建筑方法 =====

    // 建造房间：用帐墙(=)围成矩形，指定门所在的列
    // (r1,c1) = 左上角, (r2,c2) = 右下角, doorCol = 门所在列
    void buildRoom(int r1, int c1, int r2, int c2, int doorCol);

    // 建造栅栏：横向，在 row=r 行从 col=c 开始向右延伸 length 格
    void buildFence(int r, int c, int length, int doorCol, int doorCol2 = -1);

    // 放置拒马（单格障碍）
    void buildCheval(int r, int c);

    // 放置水域（横向 length 格）
    void buildWater(int r, int c, int length);

    // 建造传送门（条状，连接地图）：文字提示「去<dest><箭头>」，dest 存完整目标名。
    // dir 决定箭头指向与条延伸方向；length<=0 时取最小长度（刚好完整显示文字）。
    void buildPortal(int r, int c, PortalDir dir, const std::string& dest, int length = 0);

    // 交互回调：玩家踩上不可通行格（友方/敌方/药店/物品/跳转点/传送门）时触发，
    // 参数依次为：格子类型、单位名、所在行、所在列
    std::function<void(TileType, const std::string&, int, int)> onInteract;

    // 结果标志：由调用方在 onInteract 内设置（跳幕 / 传送）
    bool advanceTriggered = false;
    bool portalTriggered = false;
    std::string portalTarget;

private:
    std::vector<std::vector<Tile>> grid;
    int playerRow = 1;
    int playerCol = 1;

    // 玩家脚下被覆盖的原始格子（门/空地等），离开时恢复
    Tile underPlayer{ "    ", TileType::EMPTY };

    bool isValid(int row, int col) const;
};
