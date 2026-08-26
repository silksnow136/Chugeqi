#pragma once
#include<string>
#include <windows.h>
//负责控制游戏初始化以及指令处理
using namespace std;

class Game {
public:
	void run();
	//设置字体颜色
	void setColor(int colorCode) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, colorCode);
	}
private:
	bool running = true;

	void initialize();
	void gameLoop();//游戏主循环
	void gameCommand(const string& command);//处理玩家输入

	void showWelcome();
	void showHelp();
};