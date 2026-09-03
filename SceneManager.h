#pragma once
//负责故事背景显示
//负责显示剧情
//负责显示场景以及场景交互
#include "TalkManager.h"
using namespace std;

class Game;
class PharManager;
class ForgeManager;

class SceneManager {
public:
	SceneManager(Game& game);

//判断当前场景所处状态，方便添加多级系统
	enum class SceneState
	{
		ORIGIN_SCENE,	//当前场景
		TALK,			//进入对话系统,选择角色对话
		PHARMACY,		//进入药店
		FORGE			//进入锻造
	};

	void ShowBackground(int scene_id);

	void changeAuto();//自动/手动切换
	bool current_Auto();//当前的自动/手动模式
	//初步剧情存档,获得当前场景id
	int showScene_id();
	// 修改场景
	void changeScene(int scene_id);
	//场景功能显示管理
	void showSceneManager(int scene_id = 0, int branch_id = 0);
	//场景功能管理,1对话系统以及对话分支+指令系统；2.药店系统；3.锻造系统
	void sceneManager(Game& game1, int branch_id);

	void nextLine();

	// 获取当前状态
	SceneState getSceneState() const;
	// 设置当前状态
	void setSceneState(SceneState state);

	// 获得对话人物
	int getCurrentCharacter() const;
	// 设置对话人物
	void setCurrentCharacter(int character_id);

	void deleteWords(string tip);

	// 当前选择的人物
	int current_character = 0;
	//是否返回地图场景
	bool scene = true;
private:

	Game& game;

	TalkManager talkManager;
	PharManager* pharManager = nullptr;
	ForgeManager* forgeManager = nullptr;

	// 当前场景ID
	int current_scene_id = 0;

	// 当前场景状态
	SceneState current_state = SceneState::ORIGIN_SCENE;

	// 是否自动播放剧情
	bool autoPlay = false;

	// 避免地图重复打印
	bool unique_map_print = false;

	// =====================================
	// 通用辅助函数
	// =====================================
public:
	// 清屏并显示地图
	void refreshScene(int branch_id);

	// 判断并执行命令
	bool handleCommand(Game& game1, const string& command);

	// 显示当前场景背景
	void showCurrentBackground();

	//输出剧情对话
	void printWords(string tips,int color, int sleep,int sleep1 = 100);
};

