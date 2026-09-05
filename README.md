# 楚歌起 (Chugeqi)

> 霸王之陨，长乐未央 —— 一款以「楚汉相争 · 项羽垓下之围到乌江自刎」为背景的 Windows 控制台回合制 RPG。

本仓库由两个项目合并而来：

- **Chugeqi**（战斗内核）：回合制战斗、角色/技能/物品、SQLite 存档、JSON 数据加载。
- **TheFalloftheOverlord**（剧情外壳）：四幕剧情、场景、对话、地图、逐字打印。

技术栈：C++17 + CMake + 自研 JSON 解析器 + SQLite。

---

## 当前状态

可编译、可运行。四幕剧情（`data/story.json` 驱动）与回合制战斗（含敌人 AI、状态效果）均已就绪，但**剧情与战斗尚未接通**——剧情里的三处「打斗」目前仍是文字描述。

---

## 目录结构

```
Chugeqi/
├── CMakeLists.txt           # 构建配置（两个目标）
├── README.md                # 本文件
├── 开发日志.md              # 开发进度与实现细节
├── data/                    # JSON 数据
│   ├── story.json           # 四幕剧情（旁白/对话/分支）
│   ├── skill.json           # 技能池
│   ├── player.json          # 主角模板
│   ├── companion.json       # 同伴模板
│   ├── enemy_test*.json     # 敌人模板
│   └── battle_test.json     # 战斗配置（引用上述模板）
├── src/
│   ├── core/                # 平台抽象 + 数据加载（共用）
│   │   ├── console.h/.cpp   # 控制台抽象：清屏/按键/颜色/延时/编码
│   │   ├── json.h           # 轻量 JSON 解析器
│   │   └── dataLoader.h/.cpp# 统一加载：战斗 + 剧情
│   ├── combat/              # 战斗内核
│   │   ├── combatSystem.h/.cpp  # 战斗系统（回合流程/AI/状态效果）
│   │   ├── character.h/.cpp     # 角色（Character/Civil/Combatant）
│   │   ├── skill.h/.cpp         # 技能（伤害/治疗/充能）
│   │   ├── item.h/.cpp          # 物品（消耗品/装备）
│   │   ├── saveManager.h/.cpp   # SQLite 存档
│   │   ├── battleLog.h/.cpp     # 战斗日志
│   │   └── combatTestMain.cpp   # 战斗测试入口
│   ├── story/               # 剧情 / 场景
│   │   ├── Game.h/.cpp          # 游戏外壳（主循环/指令）
│   │   ├── SceneManager.h/.cpp  # 场景 / 剧情播放
│   │   ├── TalkManager.h/.cpp   # 对话系统
│   │   ├── map.h/.cpp           # 世界地图 / 场景地图
│   │   ├── BackGround.h/.cpp    # 场景背景
│   │   ├── ForgeManager.h/.cpp  # 锻造（占位）
│   │   └── PharManager.h/.cpp   # 药房（占位）
│   └── main.cpp             # 主入口（剧情）
├── archive/                 # 归档的旧设计文档（软件设计 / 需求分析）
├── docs/                    # 流程图
└── third_party/sqlite/      # SQLite 官方合并源码
```

---

## 构建与运行

环境要求：Windows、Visual Studio（含 C++ 工具链）、CMake ≥ 3.15。

```powershell
# 1. 配置（Visual Studio 生成器，x64）
cmake -S . -B build -G "Visual Studio 18 2026" -A x64

# 2. 编译（Release）
cmake --build build --config Release
```

两个可执行目标：

| 目标 | 入口 | 说明 |
|------|------|------|
| `Chugeqi.exe` | `src/main.cpp` | 主游戏：四幕剧情 |
| `combatTest.exe` | `src/combat/combatTestMain.cpp` | 战斗测试：回合制战斗 |

CMake 会在构建后自动把 `data/` 复制到各 exe 同目录。

```powershell
cd build\Release
.\Chugeqi.exe       # 跑剧情
.\combatTest.exe    # 跑战斗测试
```

---

## 功能现状

### 已实现

- **剧情**：四幕（垓下/阴陵/东城/乌江），从 `story.json` 加载；逐字打印、自动/手动播放（ESC 切换）、第二幕分支选择。
- **战斗**：回合制，玩家 + 同伴；普通攻击、伤害/治疗/充能技能、逃跑。
- **敌人 AI**：敌方会回击，按策略选技能或集火残血；我方支持 AI 托管。
- **状态效果**：灼烧 / 迟缓 / 眩晕 / 充能，统一在主循环每轮结算。
- **角色成长**：经验值与升级。
- **存档**：SQLite 三表持久化队伍状态（读档 / 存档 / 重置）。
- **平台抽象**：控制台清屏 / 按键 / 颜色 / 延时 / UTF-8 编码集中在 `console` 模块。

### 未实现 / 待办

- **剧情 ↔ 战斗衔接**：剧情里三处「打斗」尚未接入 `CombatSystem`。
- **对话内容**：`TalkManager` 里各角色对话仍是「没写」占位。
- **药房 / 锻造**：`PharManager` / `ForgeManager` 为占位实现。
- **道具使用**：`performItem` 为空实现。
- **任务系统**：未实现。

---

## 数据文件说明

- `story.json`：四幕剧情。每幕含 `lines`（`text` / `color` / `sleep` / `wait`），第二幕含 `choice`（`prompt` + `options`，每项含 `key` / `branch` / `lines`）。
- `skill.json`：技能池，`type`（damage / heal / charge）、`cost`、`power`、`healAmount`、`multiplier`、`targetStat`、`duration`、`statusEffect`、`scope` 等。
- `player.json` / `companion.json` / `enemy_test*.json`：角色模板，`baseStats[4]`、`skills`、`inventory`。
- `battle_test.json`：战斗配置，通过 `player_ref` / `companions_ref` / `enemies_ref` 引用角色模板。

---

## 更多文档

- 开发进度、实现细节、文件职能、测试方法：见 [开发日志.md](./开发日志.md)。
- 归档的旧设计文档：`archive/`（软件设计、需求分析）。
