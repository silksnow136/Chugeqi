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
	cout << "\n输入w继续游戏\n";

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

	cout << "1. 王翦\n";
	cout << "2. 汉军\n";
	cout << "3. 返回\n";
	cout << "\n输入w继续游戏\n";

	cout << "\n> ";
}
void talkScene_022() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 副将\n";
	cout << "2. 返回\n";
	cout << "\n输入w继续游戏\n";

	cout << "\n> ";
}
void talkScene_03() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 赤泉侯\n";
	cout << "2. 秦时月\n";
	cout << "3. 钟离昧\n";
	cout << "4. 二十八骑\n";
	cout << "5. 返回\n";
	cout << "\n输入w继续游戏\n";

	cout << "\n> ";
}
void talkScene_04() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 韩信\n";
	cout << "2. 汉军\n";
	cout << "3. 钟离昧\n";
	cout << "4. 二十八骑\n";
	cout << "5. 乌江亭长\n";
	cout << "6. 返回\n";
	cout << "\n输入w继续游戏\n";

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
	switch (current_character) {
	case 1:
		//王翦
		cout << "王翦的对话没写" << "\n";
		break;

	case 2:
		//汉军
		cout << "汉军的对话没写" << "\n";
		break;
	}
}
void talk_character_contnt_022(int current_character) {
	switch (current_character) {
	case 1:
		//副将
		cout << "副将的对话没写" << "\n";
		break;

	}
}
void talk_character_contnt_03(int current_character) {
	switch (current_character) {
	case 1:
		//赤泉侯
		cout << "赤泉侯的对话没写" << "\n";
		break;

	case 2:
		//秦时月
		cout << "秦时月的对话没写" << "\n";
		break;

	case 3:
		//钟离昧
		cout << "钟离昧的对话没写" << "\n";
		break;
	case 4:
		//二十八骑
		cout << "二十八骑的对话没写" << "\n";
		break;
	}
}
void talk_character_contnt_04(int current_character) {

}