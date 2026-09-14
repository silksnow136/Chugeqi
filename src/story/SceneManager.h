#pragma once
//负责故事背景显示
//负责显示剧情
//负责显示场景以及场景交互
#include "data/dataLoader.h"
#include "TalkManager.h"
#include "QuestState.h"
#include <memory>
using namespace std;

class Game;
class PharManager;

class SceneManager {
public:
	SceneManager(Game& game);
	~SceneManager();

	void ShowBackground(int scene_id);

	void changeAuto();//自动/手动切换
	bool current_Auto();//当前的自动/手动模式

	// 获取对话管理器（供地图移动交互调用）
	TalkManager& getTalkManager() { return talkManager; }

	// 支线任务进度（军心/委托/阴陵地牢 flag，跨场景共享）
	QuestState& getQuestState() { return questState; }

	// 进入药店系统（供地图移动交互调用）
	void enterPharmacy(Game& game1);

	//获得当前场景id
	int showScene_id();
	// 获得当前分支id（第二幕 A/B）
	int showBranch_id();
	// 幕次跳转旁白（ADVANCE 格触发）
	const string& getAdvancePrompt(int scene_id) const;
	// 修改场景
	void changeScene(int scene_id);
	//场景功能显示管理
	void showSceneManager(int scene_id = 0, int branch_id = 0);

	// 读档恢复：跳过幕次剧情，直接进入指定幕的地图
	void resumeScene(int scene_id, int branch_id);

	// 播放终幕结局（乌江：final 决战 + 结算）
	void playEnding();

	void nextLine();

	void deleteWords(string tip);

	//输出剧情对话
	void printWords(string tips,int color, int sleep,int sleep1 = 100);

private:

	Game& game;

	TalkManager talkManager;
	QuestState questState;   // 支线任务状态
	unique_ptr<PharManager> pharManager;

	// 当前场景ID
	int current_scene_id = 0;

	// 当前分支ID（第二幕 A/B，默认 0）
	int current_branch_id = 0;

	// 是否自动播放剧情
	bool autoPlay = false;

	// 剧情数据（构造时从 data/story/story.json 载入一次）
	std::vector<Scene> scenes;

	// 播放叙事行 / 分支选择
	bool playLines(const std::vector<StoryLine>& lines);
	bool playChoice(const Scene& scene, int& branch_id);

	bool startStoryBattle(const std::string& battleId);
};
