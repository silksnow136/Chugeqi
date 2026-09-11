# 楚歌起 (Chugeqi)

> 霸王之陨，长乐未央 —— 一款以「楚汉相争 · 项羽垓下之围到乌江自刎」为背景的控制台回合制 RPG。

本仓库由两个项目合并而来：

- **Chugeqi**（战斗内核）：回合制战斗、角色/技能/物品、SQLite 存档、JSON 数据加载。
- **TheFalloftheOverlord**（剧情外壳）：四幕剧情、场景、对话、地图、逐字打印。

技术栈：C++17 + CMake + 自研 JSON 解析器 + SQLite。

> 旧的 README 与开发日志已归档至 `archive/`（`README_旧版.md`、`开发日志_旧版.md`）。

---

## 当前状态

可编译、可运行（Windows / Linux 均兼容）。**主线剧情与战斗尚未接通**——剧情里三处「打斗」仍是文字描述；战斗目前只在独立测试入口（`mapTest`）里与地图遇敌接通。

---

## 目录结构

```
Chugeqi/
├── CMakeLists.txt            # 构建配置（7 个目标）
├── README.md                 # 本文件
├── 开发日志.md               # 开发进度与实现细节
├── data/                     # JSON 数据（静态模板）
│   ├── story.json            # 四幕剧情（旁白/对话/分支）
│   ├── skill.json            # 技能池
│   ├── item.json             # 物品池（消耗品 + 装备 + 材料）
│   ├── talk.json             # 场景内角色对话（737 行）
│   ├── player.json           # 主角模板
│   ├── companion.json        # 同伴模板
│   ├── enemy_test*.json      # 敌人模板
│   └── battle_test.json      # 战斗配置（引用上述模板）
├── src/
│   ├── core/                 # 平台抽象 + 数据加载（共用）
│   │   ├── console.h/.cpp    # 控制台抽象：清屏/按键/颜色/延时/编码/光标
│   │   ├── json.h            # 轻量 JSON 解析器
│   │   └── dataLoader.h/.cpp # 统一加载：战斗 + 剧情 + 物品/装备
│   ├── combat/               # 战斗内核
│   │   ├── combatSystem.h/.cpp  # 战斗系统（回合流程/AI/状态效果）
│   │   ├── character.h/.cpp     # 角色（Character/Civil/Combatant，含装备槽位）
│   │   ├── skill.h/.cpp         # 技能（伤害/治疗/充能）
│   │   ├── item.h/.cpp          # 物品（消耗品/装备）
│   │   ├── saveManager.h/.cpp   # SQLite 存档
│   │   ├── battleLog.h/.cpp     # 战斗日志
│   │   └── combatTestMain.cpp   # 战斗测试入口
│   ├── story/                # 剧情 / 场景
│   │   ├── Game.h/.cpp          # 游戏外壳（主循环/指令）
│   │   ├── SceneManager.h/.cpp  # 场景状态机 / 剧情播放
│   │   ├── TalkManager.h/.cpp   # 对话系统（从 talk.json 加载）
│   │   ├── MapGrid.h/.cpp       # 网格地图系统（WASD/碰撞/交互/建造）
│   │   ├── gaiXiaMap.cpp        # 第一幕「垓下营地」地图展示
│   │   ├── huaiRiverMap.cpp     # 第二幕「淮河突围」地图展示
│   │   ├── dongchengMap.cpp     # 第三幕「东城快战」地图展示
│   │   ├── wujiangMap.cpp       # 第四幕「乌江自刎」地图展示
│   │   ├── mapTestMain.cpp      # 地图移动 + 遇敌战斗测试
│   │   ├── PharManager.h/.cpp   # 药店（购买/使用药品/查看背包）
│   │   ├── ForgeManager.h/.cpp  # 锻造（占位）
│   │   ├── map.h/.cpp           # 旧版静态字符地图（遗留）
│   │   └── BackGround.h/.cpp    # 旧版场景标题（遗留）
│   └── main.cpp              # 主入口（剧情）
├── archive/                  # 归档文档（旧 README/开发日志/软件设计/需求分析）
├── docs/                     # 流程图
└── third_party/sqlite/       # SQLite 官方合并源码
```

---

## 构建与运行

环境要求：CMake ≥ 3.15；Windows（Visual Studio，含 C++ 工具链）或 Linux（g++/clang）均可。

```bash
# 1. 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 2. 编译
cmake --build build
```

七个可执行目标：

| 目标 | 入口 | 说明 |
|------|------|------|
| `Chugeqi` | `src/main.cpp` | 主游戏：四幕剧情 |
| `combatTest` | `src/combat/combatTestMain.cpp` | 战斗测试：回合制战斗 |
| `mapTest` | `src/story/mapTestMain.cpp` | 地图移动 + 遇敌接真实战斗 |
| `gaiXiaMap` | `src/story/gaiXiaMap.cpp` | 第一幕垓下营地地图展示 |
| `huaiRiverMap` | `src/story/huaiRiverMap.cpp` | 第二幕淮河突围地图展示 |
| `dongchengMap` | `src/story/dongchengMap.cpp` | 第三幕东城快战地图展示 |
| `wujiangMap` | `src/story/wujiangMap.cpp` | 第四幕乌江自刎地图展示 |

CMake 会在构建后自动把 `data/` 复制到各可执行文件同目录。

---

## 功能现状

### 已实现

- **剧情**：四幕（垓下/阴陵/东城/乌江），从 `story.json` 加载；逐字打印、自动/手动播放（ESC 切换）、按 q 加速、第二幕分支选择。
- **对话**：场景内选择角色对话，从 `talk.json` 按 `scene_id/branch_id/character_id` 加载播放。
- **战斗**：回合制，玩家 + 同伴；普通攻击、伤害/治疗/充能技能、逃跑。
- **敌人 AI**：敌方回击，按策略选技能或集火残血；我方支持 AI 托管。
- **状态效果**：灼烧 / 迟缓 / 眩晕 / 充能，统一在主循环每轮结算。
- **角色成长**：经验值与升级。
- **装备**：四槽位（防具/武器/鞋子/配饰）装配/卸下，属性加成叠加进 `getEffectiveStat`。
- **物品**：消耗品（回 HP/SP）、装备、材料三类，从 `item.json` 加载；背包界面可装配/卸下/使用。
- **药店**：金币购买、使用药品、查看背包（`PharManager`）。
- **网格地图**：`MapGrid` 支持 WASD 移动、边界/碰撞检测、按格子类型触发交互（对话/战斗/药店/锻造/拾取/门）、拟物建造（房间/栅栏/拒马/水域）、光标定位渲染防闪烁。
- **存档**：SQLite 三表（`characters`/`learned_skills`/`inventory`）持久化队伍状态。
- **平台抽象**：`console` 11 个函数（含 `moveCursor`/`setCursorVisible`/`clearToEnd`），Windows / Linux 双端。

### 未实现 / 待办

- **剧情 ↔ 战斗衔接**：主线剧情三处「打斗」未接入 `CombatSystem`（仅 `mapTest` 已接通）。
- **锻造**：`ForgeManager` 为占位；`SceneManager` 的 `FORGE` 状态未真正调用它。
- **战斗内道具使用**：`performItem` 仍为空实现（战斗外背包/药店可用）。
- **继续游戏**：主菜单「继续征途」未实现。
- **剧情存档**：`current_scene_id` 仅存内存，未持久化。
- **第四幕对话**：`SceneManager` 的 TALK 状态 `case 4` 为空（`TalkManager::talkScene04` 已写但未接线）。
- **任务系统**：未实现。
- **自动化测试**：目前为手工交互测试。

---

## 已知遗留 / 技术债

- **三套场景表现系统并存**，未整合：
  1. `BackGround.h/.cpp`（最早期文字标题，仍在 `showCurrentBackground` 使用）；
  2. `map.h/.cpp`（旧静态字符地图，主游戏实际在用）；
  3. `MapGrid` + 四张地图（新网格系统，仅独立 exe，未接入主游戏）。
- **陈旧注释**：`dataLoader.h:23`「道具未实现」、`combatSystem.h:3-6`「状态效果/充能未实现」、`SceneManager.cpp`「药店/锻造未制作」等，均与现状不符。
- **死代码**：`TalkManager::talkScene_020`（农夫）、`Game.cpp` 内「不要忘记删除」注释等。

---

## 数据文件说明

- `story.json`：四幕剧情。每幕含 `lines`（`text`/`color`/`sleep`/`wait`），第二幕含 `choice`（`prompt` + `options`，每项含 `key`/`branch`/`lines`）。
- `talk.json`：场景对话，按 `scenes[].characters[].dialogue[]` 组织，每句含 `speaker`/`text`；第二幕含 `branches`。
- `skill.json`：技能池，`type`（damage/heal/charge）、`cost`、`power`、`healAmount`、`multiplier`、`targetStat`、`duration`、`statusEffect`、`scope` 等。
- `item.json`：物品池，含 `category`（equipment/potion/material）、装备的 `slot`+`bonus[4]`、消耗品的 `healHP`/`healSP` 等。
- `player.json` / `companion.json` / `enemy_test*.json`：角色模板，`baseStats[4]`、`skills`、`inventory`。
- `battle_test.json`：战斗配置，通过 `player_ref`/`companions_ref`/`enemies_ref` 引用角色模板。

---

## 更多文档

- 开发进度、实现细节、文件职能、测试方法：见 [开发日志.md](./开发日志.md)。
- 归档的旧文档：`archive/`（旧 README、旧开发日志、软件设计、需求分析）。
