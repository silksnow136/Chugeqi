# 楚歌起 (Chugeqi)

> 霸王之陨，长乐未央 —— 一款以「楚汉相争·项羽」为背景的 Windows 控制台回合制战斗 RPG 原型。

本仓库当前是一个**可编译、可运行**的回合制战斗原型，采用 C++17 + CMake + SQLite，数据分层为「JSON 静态模板 + SQLite 存档 + 内存战斗对象」。

---

## 一、背景故事概要

故事取材自项羽人生终章「垓下之围 → 乌江自刎」，分四幕推进：

| 幕 | 时间 | 地点 | 情节 |
|----|------|------|------|
| 第一幕：四面楚歌，垓下之围 | 公元前 202 年 12 月上旬 | 垓下（今安徽灵璧东南） | 项羽与虞姬被困，兵少食尽，夜间楚歌四起，霸王悲歌《垓下歌》 |
| 第二幕：突围南逃，淮河之阻 | 12 月中旬 | 垓下 → 阴陵 | 率八百骑突围，渡淮河余百余骑；遇田夫指路，左右两条支线（右遇王翦、左陷沼泽） |
| 第三幕：东城快战，以一敌千 | 12 月下旬 | 东城 | 余二十八骑，斩将刈旗，连斩小将秦时月 |
| 第四幕：乌江自刎，天地同悲 | 12 月底 | 乌江 | 虞姬自刎，项羽冲阵连斩，最终自刎而亡，一代霸王落幕 |

主要人物初始属性（来自背景设定）：

- **项羽**：力量 90 / 敏捷 70 / 防御 80 / 最大生命 150，装备霸王枪、乌骓马
- **虞姬**：力量 40 / 敏捷 80 / 防御 60 / 最大生命 110
- **王翦**：力量 80 / 敏捷 60 / 防御 60 / 最大生命 130，装备披云枪、逐月马
- **秦时月**（小将）：力量 60 / 敏捷 40 / 防御 60 / 最大生命 110，装备长虹枪、步云马

---

## 二、需求概述

依据《需求分析》文档，本项目的核心需求如下（括号内为当前落地情况）：

- **基础属性**（主人物 / 敌人 / NPC）→ 生命值、力量、敏捷、防御 —— ✅ 已实现为 `Combatant` 的 4 维属性 + HP/SP
- **人物功能属性** → 常态：背包、查看基础属性、拾取；特殊：战斗触发、任务触发、与 NPC 对话 —— 🔶 背包/属性/战斗已具备，拾取、任务、对话仅留接口
- **幕次** → 四幕剧情 —— 🔶 仅预留数据结构，剧情脚本未接入
- **战斗方式**（主人物 / 敌人）→ 回合制 —— ✅ 已实现我方回合制，敌方 AI 未实现
- **任务事件** —— ⬜ 未实现
- **存储功能** → 游戏开始：创建 / 加载 / 离开；游戏中：存档 / 离开 —— ✅ 已实现 SQLite 存档 / 读档 / 重置

图例：✅ 已实现　🔶 部分实现　⬜ 未实现

---

## 三、当前已实现功能

- **回合制战斗**：玩家 + 同伴手动操作，普通攻击、伤害技能、治疗技能、逃跑
- **命中与伤害计算**：基于敏捷的命中率、`力量 × 威力 − 防御` 的伤害公式
- **角色成长**：经验值与升级（升级全属性 +1、回满 HP/SP）
- **技能 / 物品类层次**：伤害 / 治疗 / 充能三类技能，消耗品 / 装备两类物品
- **数据加载**：自研轻量 JSON 解析器加载技能池、角色模板、战斗配置
- **存档系统**：SQLite 三表持久化队伍状态，支持读档 / 存档 / 重置
- **战斗日志**：7 行滚动日志，空时居中显示「(暂无)」
- **平台抽象**：控制台清屏 / 按键 / 编码集中在 `console` 模块，业务代码不依赖具体平台

## 四、尚未实现（预留接口）

- 道具使用（`performItem` 为空实现）
- 状态效果结算（灼烧 / 迟缓 / 眩晕 / 充能，数据结构已就绪但未在战斗中生效）
- 敌人 AI（敌人当前不会回击）
- 充能技能的实际执行
- 同伴 AI（同伴需手动操控）
- 剧情 / 任务系统

---

## 五、目录结构

```
Chugeqi/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 本文件
├── .gitignore              # 忽略 build/、*.db、.vscode/
├── data/                   # 静态数据模板（JSON）
│   ├── skill.json          # 技能池定义
│   ├── player.json         # 主角模板
│   ├── companion.json      # 同伴模板
│   ├── enemy_test1.json    # 敌人模板 1
│   ├── enemy_test2.json    # 敌人模板 2
│   └── battle_test.json    # 战斗配置（引用上述模板）
├── src/                    # 源码
│   ├── combatTestMain.cpp  # 程序入口 / 流程编排
│   ├── combatSystem.*      # 战斗系统
│   ├── character.*         # 角色（Character / Civil / Combatant）
│   ├── skill.*             # 技能（SkillBase / Damage / Heal / Charging）
│   ├── item.*              # 物品（Item / Consumable / Equipment）
│   ├── dataLoader.*        # 静态数据加载
│   ├── saveManager.*       # SQLite 存档
│   ├── battleLog.*         # 战斗日志
│   ├── console.*           # 平台抽象层
│   └── json.h              # 轻量 JSON 解析器
└── third_party/sqlite/     # SQLite 官方合并源码（sqlite3.c / sqlite3.h）
```

---

## 六、构建与运行

环境要求：Windows、Visual Studio（含 C++ 工具链）、CMake ≥ 3.15。

```powershell
# 1. 配置（用 Visual Studio 生成器，架构 x64）
cmake -S . -B build -G "Visual Studio 17 2022" -A x64

# 2. 编译（Release）
cmake --build build --config Release
```

编译产物在 `build/Release/Chugeqi.exe`。CMake 会在构建后自动把 `data/` 复制到 exe 同目录，因此运行时需保证相对路径 `data/` 存在（默认在 `build/Release/` 下运行即可）。

```powershell
cd build\Release
.\Chugeqi.exe
```

> 说明：`save.db` 会在运行目录生成，用于保存队伍状态。

---

## 七、数据文件说明

静态模板均为 JSON，键值结构清晰：

- `skill.json`：以技能 id 为键，字段含 `type`（damage / heal / charge）、`name`、`cost`（SP 消耗）、`power`（威力）、`healAmount`、`scope`（single / all）、`statusEffect`、`hitRate` 等。
- `player.json` / `companion.json` / `enemy_testN.json`：角色模板，含 `id`、`name`、`level`、`hp`、`sp`、`exp`、`baseStats[4]`（力量/魔力/耐力/敏捷）、`skills`（技能 id 数组）、`inventory`。
- `battle_test.json`：战斗配置，通过 `player_ref` / `companions_ref` / `enemies_ref` 引用角色模板文件，并含 `first_side`、`disable_run`、`disable_items` 等开关。

---

## 八、存档说明

存档使用 SQLite（`save.db`），三张表：

- `characters`：角色基础状态（id / name / level / hp / sp / exp / 四维属性）
- `learned_skills`：角色已学技能（character_id + slot + skill_id）
- `inventory`：角色持有物品（character_id + item_id + count）

首次运行时自动从 JSON 模板建档；每场战斗结束写回存档，并询问是否重置（按 `Y` 清空存档，下次运行重新建档）。

---

## 九、更多文档

- 各功能实现细节、文件职责、数据结构、JSON 与 SQL 入门讲解、测试方法：见 [开发日志.md](./开发日志.md)。
