#pragma once
// ---------------------------------------------------------------------------
// 网格地图系统：WASD 控制项羽移动，边界检测，碰撞触发交互
//
// 设计：
//   - 二维网格，每格为一个 Tile（显示字符串 + 类型 + 名称）
//   - 每格固定占 4 个控制台列宽（= 2 个中文字符），不足补空格
//   - 玩家（项羽）用 WASD 移动，每次移动一格
//   - 不能越过边界墙，不能覆盖其他文字单位
//   - 试图覆盖单位时触发交互：友方→对话，敌方→战斗，药店→购买，铁匠铺→锻造
// ---------------------------------------------------------------------------

#include <vector>
#include <string>
#include <functional>

enum class TileType {
    EMPTY,      // 可通行空地（空格）
    WALL,       // 边界/墙壁（█）
    PLAYER,     // 项羽（玩家，项羽）
    FRIEND,     // 友方单位 → 触发对话
    ENEMY,      // 敌方单位 → 触发战斗
    PHARMACY,   // 药店 → 触发购买
    FORGE       // 铁匠铺 → 触发锻造
};

struct Tile {
    std::string display;    // 显示字符串（可多字符，如"虞姬"）
    TileType type = TileType::EMPTY;
    std::string name;       // 单位名称（交互时显示）

    Tile() = default;
    Tile(const std::string& d, TileType t, const std::string& n = "")
        : display(d), type(t), name(n) {}
};

class MapGrid {
public:
    // 构造：指定网格大小，自动用墙壁围边框
    MapGrid(int rows, int cols);

    // 设置某格内容（不覆盖玩家位置）
    void setTile(int row, int col, const std::string& display, TileType type, const std::string& name = "");

    // 设置玩家初始位置
    void setPlayer(int row, int col);

    // 渲染整个地图到控制台
    void render() const;

    // WASD 移动：'w'=上 'a'=左 's'=下 'd'=右
    // 返回 true 表示触发了交互（调用方可据此进入对话/战斗等）
    bool move(char direction);

    // 移动后若触发交互，获取交互类型和单位名称
    TileType getInteractionType() const { return interactionType; }
    const std::string& getInteractionName() const { return interactionName; }

    // 获取玩家当前位置
    int getPlayerRow() const { return playerRow; }
    int getPlayerCol() const { return playerCol; }

    // 交互回调（测试/集成时设置）
    std::function<void(const std::string&)> onTalk;       // 友方对话
    std::function<void(const std::string&)> onBattle;     // 敌方战斗
    std::function<void(const std::string&)> onPharmacy;   // 药店
    std::function<void(const std::string&)> onForge;      // 铁匠铺

private:
    std::vector<std::vector<Tile>> grid;
    int playerRow = 1;
    int playerCol = 1;
    TileType interactionType = TileType::EMPTY;
    std::string interactionName;

    bool isValid(int row, int col) const;
    void triggerInteraction(TileType type, const std::string& name);
};
