#include "Game.h"
#include<iostream>
#include "map.h"
#include"SceneManager.h"

Game::Game(): sceneManager(*this){}//创建 SceneManager 的时候，把当前这个 Game 对象传给它

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
	if (command == "1" && scene_id == 0) {
		//"开始游戏(剧情制作完成)";//不要忘记删除！！！！！！！
		scene_id = 1;
		sceneManager.ShowBackground(scene_id);
	}
	else if (command == "2" && scene_id == 0) {
		cout << "继续游戏（未制作完成）"<<"\n";//不要忘记删除！！！！！！！
		sceneManager.ShowBackground(sceneManager.showScene_id()+1);
	}
	else if (command == "help" || (command == "4" && scene_id == 0)) {
		showHelp();
	}
	else if (command == "quit" || (command == "3" && scene_id == 0)) {
		running = false;
		cout << "感谢游玩。"<<"\n";
		exit(0);
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
		if (scene_id > 4) {
			scene_id = 4;
		}
		sceneManager.ShowBackground(scene_id);
	}
	else if (command == "north" || command == "n" || command == "N") {
		scene_id--;
		if (scene_id < 0) {
			scene_id = 0;
		}
		sceneManager.ShowBackground(scene_id);
	}
	else if (command == "auto") {
		if (!sceneManager.current_Auto()) {
			sceneManager.changeAuto();
		}
	}
	else if (command == "manual") {
		if (sceneManager.current_Auto()) {
			sceneManager.changeAuto();
		}
	}
	else if (command == "start") {
		scene_id = 0;
		showWelcome();
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
		<< "  " << "help     " << "     " << "查看帮助" << "\n"
		<< "  " << "quit     " << "     " << "退出游戏" << "\n"
		<< "  " << "map      " << "     " << "查看地图" << "\n"
		<< "  " << "south(s)(S)" << "   " << "继续剧情" << "\n"
		<< "  " << "north(n)(N)" << "   " << "回退剧情" << "\n"
		<< "  " << "auto  " << "        " << "自动播放剧情" << "\n"
		<< "  " << "manual" << "        " << "手动播放剧情" << "\n"
		//<< "  " << "ESC   " << "      " << "切换自动/手动播放剧情" << "\n"
		<< "  " << "start " << "        " << "开始界面" << "\n"
		
		<< "===========================" << "\n";
}

