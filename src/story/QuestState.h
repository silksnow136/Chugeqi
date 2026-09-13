#pragma once
// ---------------------------------------------------------------------------
// QuestState — 支线任务进度（跨场景地图共享，由 SceneManager 持有）
//
// 支线一「收拢军心」（第一幕）：
//   q1 寻回逃兵 / q2 巡南门 / q3 分配粮草
//   状态：0=未接  1=进行中  2=可交付  3=已完成
// 支线二「阴陵迷境」（第二幕）：
//   yinlingUnlocked 田夫对话后开启入口
//   guanyingDefeated 击败灌婴后开启返回传送门
// ---------------------------------------------------------------------------

struct QuestState {
    // ===== 支线一 =====
    int morale = 50;          // 军心
    int q1 = 0;               // 老兵·寻回逃兵
    int q2 = 0;               // 粮官·巡南门
    int q3 = 0;               // 粮仓·分配粮草
    int q3choice = 0;         // 1=优先士兵 2=优先战马 3=留存突围
    int deserters = 0;        // 已劝回逃兵数（0~3）
    bool deserterTalked[3] = { false, false, false };

    // ===== 支线二 =====
    bool yinlingUnlocked = false;  // 田夫指路后开启阴陵入口
    bool guanyingDefeated = false; // 灌婴已被击败
    bool raincoatWarned = false;   // 蓑衣寒水提示本层已给过
};
