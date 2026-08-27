#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include <conio.h> // 控制台无回显按键读取

void talkManager(int scene_id = 0) {
	switch (scene_id) {
	case 1:

		break;

	case  2:
		break;

	case 3:
		break;

	case 4:
		break;

	default:
		break;
	}
}

//药房
void phar() {
	cout << "\n";
	cout << "药房功能暂未开放。\n";
}

//锻造
void forge() {
	cout << "\n";
	cout << "锻造功能暂未开放。\n";

}

//大世界场景
void backGround_01() {
	cout << "\n";
	cout << "================================\n";
	cout << "            垓下大帐\n";
	cout << "================================\n";
}
void backGround_02() {
	cout << "\n";
	cout << "================================\n";
	cout << "              淮河\n";
	cout << "================================\n";
}
void backGround_03() {
	cout << "\n";
	cout << "================================\n";
	cout << "              东城\n";
	cout << "================================\n";
}
void backGround_04() {
	cout << "\n";
	cout << "================================\n";
	cout << "              乌江\n";
	cout << "================================\n";
}

//对话系统
void talkScene_01() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 小卒a\n";
	cout << "2. 虞姬\n";
	cout << "3. 返回\n";

	cout << "\n> ";
}
void talkScene_020() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. \n";
	cout << "2. \n";
	cout << "3. 返回\n";

	cout << "\n> ";
}
void talkScene_021() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. \n";
	cout << "2. \n";
	cout << "3. 返回\n";

	cout << "\n> ";
}
void talkScene_022() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. \n";
	cout << "2. \n";
	cout << "3. 返回\n";

	cout << "\n> ";
}
void talkScene_03() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 项羽\n";
	cout << "2. 虞姬\n";
	cout << "3. 返回\n";

	cout << "\n> ";
}
void talkScene_04() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 项羽\n";
	cout << "2. 虞姬\n";
	cout << "3. 返回\n";

	cout << "\n> ";
}


void talk_character_contnt_01(int current_character) {
	switch (current_character) {
	case 1:
		//小卒a
		cout << "小卒a的对话没写"<<"\n";
		break;

	case 2:
		//虞姬
		cout << "虞姬的对话没写"<<"\n";
		break;
	}
}
void talk_character_contnt_020(int current_character) {

}
void talk_character_contnt_021(int current_character) {

}
void talk_character_contnt_022(int current_character) {

}
void talk_character_contnt_03(int current_character) {

}
void talk_character_contnt_04(int current_character) {

}