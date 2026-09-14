# 楚歌起 (Chugeqi)

> 霸王之陨，长乐未央 —— 一款以「楚汉相争 · 项羽垓下之围到乌江自刎」为背景的控制台回合制 RPG。

本仓库由两个项目合并而来：

- **Chugeqi**（战斗内核）：回合制战斗、角色/技能/物品、JSON 存档、JSON 数据加载。
- **TheFalloftheOverlord**（剧情外壳）：四幕剧情、场景、对话、地图、逐字打印。

技术栈：C++17 + CMake + 自研 JSON 解析器。存档为 JSON 文件（`saves/`），不再依赖 SQLite。

> 历史文档已归档至 `archive/`（`README_旧版.md`、`README_2026-09-13.md`、`README_2026-09-14.md`、`开发日志_旧版.md`、`开发日志_2026-09-13.md`、`开发日志_2026-09-14.md`、`软件设计.md`、`需求分析.md`）。

---

## 当前状态

可编译、可运行（Windows / Linux 均兼容）。**主线剧情、战斗、网格地图、支线任务已全部接通**：

- 三处「打斗」（第二幕王翦、第三幕秦时月、第四幕最终决战）通过 `story.json` 的 `battle`/`battleId` 字段接入 `CombatSystem`。
- 四幕网格地图（垓下营地 / 淮河 / 东城 / 乌江）由 `MapLayouts` 摆布局、`SceneMap` 跑交互，支持**传送门地图链**（垓下↔营外荒郊、淮河↔阴陵一/二/三）。
- 两套**支线任务**（第一幕「收拢军心」、第二幕「阴陵迷境」）由 `SideQuest` + `QuestState` 驱动。
- 存读档界面已实现（JSON 双槽位），战斗内道具使用已实现。

---

## 目录结构

```
Chugeqi/
├── CMakeLists.txt            # 构建配置（3 个目标）
├── README.md                 # 本文件
├── 开发日志.md               # 开发进度与实现细节
├── data/                     # JSON 数据（静态模板）
│   ├── story.json            # 四幕剧情（旁白/对话/分支/战斗触发/幕次旁白 advance）
│   ├── talk.json             # 场景对话（simple 简单对话 + scenes 按 id 索引）
│   ├── skill.json            # 技能池
│   ├── item.json             # 物品池（消耗品 + 装备 + 材料）
│   ├── player.json           # 主角模板
│   ├── companion.json        # 同伴模板
│   ├── battle_*.json         # 战斗配置（battle_test / battle_wangjian / battle_qinshiyue / battle_final）
│   └── enemy_test*.json / wangjian.json / qinshiyue.json / lvsheng.json / yangwu.json / yangxi.json  # 敌人模板
├── src/
│   ├── core/                 # 平台抽象（共用）
│   │   ├── console.h/.cpp    # 控制台抽象：清屏/按键/颜色/延时/编码/光标/raw 模式/整行输入
│   │   └── json.h            # 轻量 JSON 解析器
│   ├── data/                 # 数据加载 + 存档
│   │   ├── dataLoader.h/.cpp # 统一加载：战斗 + 剧情 + 物品/装备
│   │   └── saveManager.h/.cpp# JSON 存档（2 槽位，存队伍 + 剧情进度）
│   ├── combat/               # 战斗内核
│   │   ├── combatSystem.h/.cpp       # 战斗系统（回合流程/状态效果/核心）
│   │   ├── combatSystemUI.cpp        # 界面（渲染/菜单/目标选择/状态显示）
│   │   ├── combatSystemTurns.cpp     # 回合处理（玩家/同伴/敌方/AI 托管）
│   │   ├── combatSystemActions.cpp   # 行动执行（攻击/技能/道具/逃跑）
│   │   ├── character.h/.cpp          # 角色（Character/Combatant，含装备槽位/maxHP/maxSP）
│   │   ├── skill.h/.cpp              # 技能（伤害/治疗/充能）
│   │   ├── item.h/.cpp               # 物品（消耗品/装备）
│   │   ├── battleLog.h/.cpp          # 战斗日志
│   │   └── combatTestMain.cpp        # 战斗测试入口
│   └── story/                # 剧情 / 场景
│       ├── Game.h/.cpp              # 游戏外壳（主循环/指令/存读档）
│       ├── SceneManager.h/.cpp      # 剧情播放/幕次跳转/战斗接入
│       ├── SceneMap.h/.cpp          # 场景地图交互循环（WASD，回调接入各模块）
│       ├── MapGrid.h/.cpp           # 网格地图系统（WASD/碰撞/交互/建造/传送门）
│       ├── MapLayouts.h/.cpp        # 四幕主线地图 + 支线子地图布局
│       ├── TalkManager.h/.cpp       # 对话系统（从 talk.json 加载）
│       ├── backpack.h/.cpp          # 背包/物品工具（装配/卸下/使用/属性查看）
│       ├── SideQuest.h/.cpp         # 支线任务逻辑（收拢军心 / 阴陵迷境）
│       ├── QuestState.h             # 支线任务进度状态
│       ├── PharManager.h/.cpp       # 药店（购买/使用药品/查看背包）
│       └── portalTestMain.cpp       # 传送门地图链测试入口
├── archive/                  # 归档文档（历史 README/开发日志/软件设计/需求分析）
└── docs/                     # 流程图
```

---

## 构建与运行

### Windows

环境要求：Visual Studio（含 C++ 工具链）、CMake ≥ 3.15。

```powershell
# 1. 配置（Visual Studio 生成器，x64）
cmake -S . -B build -G "Visual Studio 18 2026" -A x64

# 2. 编译（Release）
cmake --build build --config Release
```

### Linux

环境要求：g++（支持 C++17）、CMake ≥ 3.15、Ninja（可选）。

```bash
# Debian/Ubuntu 安装依赖
sudo apt install build-essential cmake ninja-build

# 1. 配置（Ninja 生成器，Release）
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 2. 编译
cmake --build build
```

> Linux 下可执行文件无扩展名，产物在 `build/` 目录（如 `build/Chugeqi`）。
> 在项目根目录运行，程序会加载 `data/` 下的数据（构建后 CMake 也会自动把 `data/` 复制到 `build/`）。

三个可执行目标：

| 目标 | 入口 | 说明 |
|------|------|------|
| `Chugeqi` | `src/main.cpp` | 主游戏：四幕剧情 + 战斗 + 地图交互 + 支线 |
| `combatTest` | `src/combat/combatTestMain.cpp` | 战斗测试：回合制战斗 |
| `portalTest` | `src/story/portalTestMain.cpp` | 传送门地图链测试（6 张地图上下左右互通） |

CMake 会在构建后自动把 `data/` 复制到各可执行文件同目录。

Windows 运行：

```powershell
cd build\Release
.\Chugeqi.exe       # 跑剧情
.\combatTest.exe    # 跑战斗测试
.\portalTest.exe    # 跑传送门地图链测试
```

Linux 运行：

```bash
./build/Chugeqi     # 跑剧情
./build/combatTest  # 跑战斗测试
./build/portalTest  # 跑传送门地图链测试
```

---

## 功能现状

### 已实现

- **剧情**：四幕（垓下/淮河/东城/乌江），从 `story.json` 加载；逐字打印、自动/手动播放（ESC 切换）、按 q 加速、第二幕分支选择、幕次跳转旁白（`advance` 字段）。
- **剧情 ↔ 战斗衔接**：`story.json` 中 `battle`/`battleId` 触发的三场战斗（王翦/秦时月/最终决战）接入 `CombatSystem`；战斗失败中断后续剧情。
- **对话**：场景内选择角色对话，从 `talk.json` 加载——「`simple` 表」按 NPC 名播放单句、「`scenes` 表」按 `scene_id/branch_id/character_id` 播放多句；另有支线专属对话走 `SideQuest`。
- **战斗**：回合制，玩家 + 同伴；普通攻击、伤害/治疗/充能技能、逃跑（可禁用）、战斗内使用道具。
- **敌人 AI**：敌方回击，按策略选技能或集火残血；我方支持 AI 托管。
- **状态效果**：灼烧 / 迟缓 / 眩晕 / 充能，统一在主循环每轮结算。
- **角色成长**：经验值与升级；`maxHP`/`maxSP` 随等级成长并接入存档。
- **装备**：四槽位（防具/武器/鞋子/配饰）装配/卸下，属性加成叠加进 `getEffectiveStat`。
- **物品**：消耗品（回 HP/SP）、装备、材料三类，从 `item.json` 加载；背包界面（`backpack`）可装配/卸下/使用/查看属性。
- **药店**：金币购买、使用药品、查看背包（`PharManager`）。
- **网格地图**：`MapGrid` 支持 WASD 移动、边界/碰撞检测、按格子类型触发交互（对话/战斗/药店/拾取/门/幕次跳转/传送门）、拟物建造（房间/栅栏/拒马/水域/传送门）、光标定位渲染防闪烁。
- **地图接入主游戏**：`SceneMap` + `MapLayouts` 将四幕地图接入主流程，含 `ADVANCE` 幕次跳转（帅帐/北渡/突围/渡船）；地图内 `B` 背包、`ESC` 退出。
- **传送门地图链**：`PORTAL` 格 + `buildPortal` 支持上下左右链式传送，衔接子地图（营外荒郊 / 阴陵一/二/三），进入点与返回点成对。
- **支线任务**（`SideQuest` + `QuestState`）：
  - 支线一「收拢军心」（第一幕）：老兵寻回逃兵、粮官巡南门、粮仓分配粮草，影响军心值与突围旁白。
  - 支线二「阴陵迷境」（第二幕）：田夫解锁入口、渡河图显浅滩、蓑衣避寒水、击败灌婴 BOSS 开启归路。
- **存档**：JSON 双槽位（`saves/save_1.json`、`save_2.json`），持久化队伍状态（HP/SP/maxHP/maxSP/等级/经验/技能/背包/装备）+ 剧情进度（scene/branch/gold），提供存读档界面（覆盖需确认）。
- **平台抽象**：`console` 15 个函数（`init`/`clearScreen`/`readKey`/`pause`/`pauseEsc`/`setColor`/`sleep`/`kbhit`/`moveCursor`/`setCursorVisible`/`clearToEnd`/`enterRaw`/`restoreCanonical`/`drainInput`/`readLine`），Windows / Linux 双端。

### 未实现 / 待办

- **地图遇敌战斗按名匹配**：`SceneMap` 的遭遇战目前统一走 `battle_test.json`，尚未按 NPC 名称匹配对应战斗配置。
- **支线进度入存档**：`QuestState`（军心/委托/阴陵 flag）目前仅存于内存，读档后不恢复。
- **任务系统**：未实现（仅有上述两套硬编码支线）。
- **自动化测试**：目前为手工交互测试。

---

## 已知遗留 / 技术债

- **地图遭遇战配置单一**：所有地图遇敌都复用 `battle_test.json`（`SceneMap.cpp`），且不携带同伴。
- **支线状态不持久化**：`saveManager` 只存队伍 + `scene`/`branch`/`gold`，`QuestState` 不落盘。
- **陈旧注释**（与现状不符）：`dataLoader.h:15-16` 分层说明仍写「适合 SQL/SQLite」，实际存档已改为 JSON。
- **残留文件**：仓库根目录 `save.db`（早期 SQLite 存档）、`build/` 下已弃用的旧目标二进制（`mapTest`/`gaiXiaMap`/`huaiRiverMap`/`dongchengMap`/`wujiangMap`），均已 gitignore，不影响构建。

---

## 数据文件说明

- `story.json`：四幕剧情。每幕含 `advance`（幕次跳转旁白）与 `lines`（`text`/`color`/`sleep`/`wait`/`battle`/`battleId`），第二幕含 `choice`（`prompt` + `options`，每项含 `key`/`branch`/`lines`）。
- `talk.json`：两层结构——`simple` 表（NPC 名 → 单句文本，供未接入主对话的 NPC）与 `scenes[]`（`scene_id`/`branch_id`/`characters[]`，每句含 `speaker`/`text`）。
- `skill.json`：技能池，`type`（damage/heal/charge）、`cost`、`power`、`healAmount`、`multiplier`、`targetStat`、`duration`、`statusEffect`、`scope` 等。
- `item.json`：物品池，含 `category`（equipment/potion/material）、装备的 `slot`+`bonus[4]`、消耗品的 `healHP`/`healSP` 等。
- `player.json` / `companion.json` / 各敌人模板：角色模板，`baseStats[4]`、`maxHp`/`maxSp`、`skills`、`inventory`。
- `battle_*.json`：战斗配置，通过 `player_ref`/`companions_ref`/`enemies_ref` 引用角色模板，含 `first_side`/`disable_run`/`disable_items`。

---

## 更多文档

- 开发进度、实现细节、文件职能、测试方法：见 [开发日志.md](./开发日志.md)。
- 归档的历史文档：`archive/`（旧 README、旧开发日志、软件设计、需求分析）。
