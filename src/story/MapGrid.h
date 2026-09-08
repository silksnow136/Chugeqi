#pragma once
// ---------------------------------------------------------------------------
// 网格地图系统：WASD 控制项羽移动，边界检测，碰撞触发交互
//
// 设计：
//   - 二维网格，每格为一个 Tile（显示字符串 + 类型 + 名称）
//   - 每格固定占 4 个控制台列宽（= 2 个中文字符），不足补空格
//   - 玩家（项羽）用 WASD 移动，每次移动一格
//   - 不能越过边界墙，不能覆盖其他文字单位
//   - 试图覆盖单位时触发交互：友方→对话，敌方→战斗，药店→购买，铁匠铺→锻造，物品→拾取
//   - 门(DOOR)可通行，玩家走过后恢复原样
// ---------------------------------------------------------------------------

#include <vector>
#include <string>
#include <functional>
#include "combat/character.h"
#include "combat/item.h"

enum class TileType {
    EMPTY,      // 可通行空地（空格）
    WALL,       // 边界/墙壁（█）
    PLAYER,     // 项羽（玩家）
    FRIEND,     // 友方单位 → 触发对话
    ENEMY,      // 敌方单位 → 触发战斗
    PHARMACY,   // 药店 → 触发购买
    FORGE,      // 铁匠铺 → 触发锻造
    DOOR,       // 门 → 可通行，走过后恢复
    ITEM        // 物品 → 拾取后消失
};

struct Tile {
    std::string display;    // 显示字符串
    TileType type = TileType::EMPTY;
    std::string name;       // 单位名称

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

    // 渲染整个地图到控制台（使用光标定位避免重绘闪烁/重复）
    void render() const;

    // WASD 移动
    bool move(char direction);

    // 获取交互类型和名称
    TileType getInteractionType() const { return interactionType; }
    const std::string& getInteractionName() const { return interactionName; }

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

    // 交互回调
    std::function<void(const std::string&)> onTalk;
    std::function<void(const std::string&)> onBattle;
    std::function<void(const std::string&)> onPharmacy;
    std::function<void(const std::string&)> onForge;
    std::function<void(const std::string&)> onItem;

private:
    std::vector<std::vector<Tile>> grid;
    int playerRow = 1;
    int playerCol = 1;
    TileType interactionType = TileType::EMPTY;
    std::string interactionName;

    // 玩家脚下被覆盖的原始格子（门/空地等），离开时恢复
    Tile underPlayer{ "    ", TileType::EMPTY };

    mutable bool firstRender = true;  // 首屏用 cls 清屏，后续用光标定位覆盖

    bool isValid(int row, int col) const;
    void triggerInteraction(TileType type, const std::string& name);
};

// =========================================================================
// 通用交互工具（地图场景共用：背包界面/拾取/命令输入）
// =========================================================================

// 背包条目（分组显示用）
struct BagEntry {
    std::string itemId;
    std::string name;
    int count;
    std::string category; // equipment 装备 / potion 药水 / material 材料
    int slot;             // 装备槽位，非装备为 -1
};

// 按名称在物品池中查找物品（地图物品格常用中文名标注）
const Item* findItemByName(const ItemPool& pool, const std::string& name);

// 读取一行命令（逐字符读取并回显；回车结束，ESC 返回空串）
std::string readCommandLine();

// 收集背包条目并按 装备->药水->材料 排序
void collectBagEntries(Combatant* player, const ItemPool& pool, std::vector<BagEntry>& out);

// 背包界面：显示当前装备与分类物品清单；
// equip+编号 装配 / unequip+编号 卸下 / use+编号 使用药水；回车或ESC返回地图
void showBackpack(Combatant* player, const ItemPool& pool);
