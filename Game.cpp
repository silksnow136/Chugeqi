#include "Game.h"
#include<iostream>
#include "map.h"
#include"SceneManager.h"

void setColor(int colorCode) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorCode);
}

void Game::run() {
	initialize();
	gameLoop();
}

void Game::initialize() {
	showWelcome();
}

void Game::gameLoop() {
	string command;
	while (running) {
		cout << "\n>";
		getline(cin, command);
		gameCommand(command);
	}
}

void Game::gameCommand(const string& command) {
	if (command == "1") {
		//"开始游戏(剧情制作完成)";//不要忘记删除！！！！！！！
		scene_id = 1;
		ShowBackground(scene_id);
	}
	else if (command == "2") {
		cout << "继续游戏（未制作）";//不要忘记删除！！！！！！！
	}
	else if (command == "help" || command == "4") {
		showHelp();
	}
	else if (command == "quit" || command == "3") {
		running = false;
		cout << "感谢游玩。"<<"\n";
	}
	else if (command.empty()) {
		return;
	}
	else if (command == "map") {
		setColor(3);
		map();
		setColor(14);
	}
	else if (command == "south" || command =="w" || command == "W") {
		scene_id++;
		ShowBackground(scene_id);
	}
	else if (command == "north" || command == "n" || command == "N") {
		scene_id--;
		ShowBackground(scene_id);
	}
	else if (command == "auto") {
		if (!current_Auto()) {
			chageAuto();
		}
	}
	else if (command == "manual") {
		if (current_Auto()) {
			chageAuto();
		}
	}
	else {
		cout << "未知指令：" << command << "\n";
		cout << "请输入help查看帮助" << "\n";
	}
}

void Game::showWelcome(){
	setColor(4);
	cout << "========================================" << "\n"
		<< "       楚歌起--霸王之陨，长乐未央    " << "\n"
		<< "========================================" << "\n";
	setColor(4);
	cout<< "     力拔山兮气盖世，破釜沉舟破强秦。" << "\n"
		<< "     乌江耻渡千秋义，霸业虽忘骨亦雄。" << "\n";
	setColor(14);
	cout << "\n";
	cout << "1.开始游戏" << "   " << "2.继续征途" << "   " << "3.退出游戏" <<"   " << "4.帮助";
	
}

void Game::showHelp() {
	cout << "\n" << "========== 指令 ==========" << "\n"
		<< "  " << "help  " << "     " << "查看帮助" << "\n"
		<< "  " << "quit  " << "     " << "退出游戏" << "\n"
		<< "  " << "map   " << "     " << "查看地图" << "\n"
		<< "  " << "south " << "     " << "继续剧情" << "\n"
		<< "  " << "north " << "     " << "回退剧情" << "\n"
		<< "  " << "auto  " << "     " << "自动播放剧情" << "\n"
		<< "  " << "manual" << "     " << "手动播放剧情" << "\n"
		
		<< "===========================" << "\n";
}