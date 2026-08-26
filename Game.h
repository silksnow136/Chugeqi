#pragma once
#include<string>
#include <windows.h>
//负责控制游戏初始化以及指令处理
//新增：剧情流程管理
using namespace std;

//设置字体颜色
void setColor(int colorCode);

class Game {
public:
	void run();
	
private:
	bool running = true;

	void initialize();
	void gameLoop();//游戏主循环
	void gameCommand(const string& command);//处理玩家输入

	void showWelcome();
	void showHelp();

	int scene_id = 0;
};