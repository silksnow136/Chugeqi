#pragma once
#include<string>
#include "SceneManager.h"
//负责控制游戏初始化以及指令处理
//新增：剧情流程管理
using namespace std;

class Game {
public:
	Game();
	void run();
	void gameCommand(const string& command);//处理玩家输入
private:
	bool running = true;

	void initialize();
	void gameLoop();//游戏主循环
	

	void showWelcome();
	void showHelp();

	int scene_id = 0;

	SceneManager sceneManager;
};
