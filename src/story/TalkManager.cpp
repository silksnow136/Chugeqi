#include<iostream>
#include"Game.h"
#include "TalkManager.h"
#include "SceneManager.h"
#include "core/console.h"
TalkManager::TalkManager()
{
	sceneManager = nullptr;
}

void TalkManager::setSceneManager(SceneManager* manager)
{
	sceneManager = manager;
}

//第一幕
void TalkManager::talkScene01(Game& game1, int branch_id) {
	talkScene_01();
	string sceneCommand1;
	cout << "\n> ";
	cin >> sceneCommand1;
	if (sceneCommand1 == "1" || sceneCommand1 == "2" || sceneCommand1 == "3") {
		int num = stoi(sceneCommand1);//将string转化为int
		if (num == 3) {
			//返回
			sceneManager->setSceneState(SceneManager::SceneState::ORIGIN_SCENE);
		}
		else {
			//与小兵/虞姬对话
			talkCharacter_01(num, branch_id);
		}
	}
	else {
		sceneManager->scene = sceneManager->handleCommand(game1, sceneCommand1);
		
	}
}

//第二章
void TalkManager::talkScene02(Game& game1, int branch_id) {
	if (branch_id == 1) {
		talkScene_021();
		string sceneCommand1;
		cout << "\n> ";
		cin >> sceneCommand1;
		if (sceneCommand1 == "1" || sceneCommand1 == "2" || sceneCommand1 == "3") {
			int num = stoi(sceneCommand1);//将string转化为int
			if (num == 3) {
				//返回
				sceneManager->setSceneState(SceneManager::SceneState::ORIGIN_SCENE);
			}
			else {
				//王翦/汉军
				talkCharacter_02(num, branch_id);
			}
		}
		else {
			sceneManager->scene = sceneManager->handleCommand(game1, sceneCommand1);
			
		}
	}
	else {
		talkScene_022();
		string sceneCommand1;
		cout << "\n> ";
		cin >> sceneCommand1;
		if (sceneCommand1 == "1" || sceneCommand1 == "2") {
			int num = stoi(sceneCommand1);//将string转化为int
			if (num == 2) {
				//返回
				sceneManager->setSceneState(SceneManager::SceneState::ORIGIN_SCENE);
			}
			else {
				//副将
				talkCharacter_02(num, branch_id);
			}
		}
		else {
			sceneManager->scene = sceneManager->handleCommand(game1, sceneCommand1);
			
		}
	}
}

//第三章
void TalkManager::talkScene03(Game& game1, int branch_id) {
	talkScene_03();
	string sceneCommand1;
	cout << "\n> ";
	cin >> sceneCommand1;
	if (sceneCommand1 == "1" || sceneCommand1 == "2" || sceneCommand1 == "3" || sceneCommand1 == "4" || sceneCommand1 == "5") {
		int num = stoi(sceneCommand1);//将string转化为int
		if (num == 5) {
			//返回
			sceneManager->setSceneState(SceneManager::SceneState::ORIGIN_SCENE);
		}
		else {
			//与赤泉侯/秦时月/钟离昧/二十八骑对话
			talkCharacter_03(num, branch_id);
		}
	}
	else {
		sceneManager->scene = sceneManager->handleCommand(game1, sceneCommand1);
	}
}

void TalkManager::waitForReturn() {
	string tip1 = "输入任意按键返回";
	cout << tip1;
	console::pause();
	sceneManager->deleteWords(tip1);

}
//第一幕对话播放
void TalkManager::talkCharacter_01(int character_id, int branch_id) {
	
	// 刷新场景
	sceneManager->refreshScene(branch_id);
	// 设置当前人物
	sceneManager->setCurrentCharacter(character_id);
	// 播放人物对话
	talk_character_contnt_01(character_id);

	waitForReturn();
}
//第二章对话播放
void TalkManager::talkCharacter_02(int character_id, int branch_id) {
	
	// 刷新场景
	sceneManager->refreshScene(branch_id);
	// 设置当前人物
	sceneManager->setCurrentCharacter(character_id);
	// 播放人物对话
	if (branch_id == 1) {
		talk_character_contnt_021(character_id);
	}
	else {
		talk_character_contnt_022(character_id);
	}

	waitForReturn();
}
//第三幕对话播放
void TalkManager::talkCharacter_03(int character_id, int branch_id) {
	
	// 刷新场景
	sceneManager->refreshScene(branch_id);
	// 设置当前人物
	sceneManager->setCurrentCharacter(character_id);
	// 播放人物对话
	talk_character_contnt_03(character_id);

	waitForReturn();
}
//对话系统
void TalkManager::talkScene_01() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 小卒a\n";
	cout << "2. 虞姬\n";
	cout << "3. 返回\n";
	cout << "\n输入w继续游戏\n";

}
void TalkManager::talkScene_020() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. \n";
	cout << "2. \n";
	cout << "3. 返回\n";
	cout << "\n输入w继续游戏\n";

}
void TalkManager::talkScene_021() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 王翦\n";
	cout << "2. 汉军\n";
	cout << "3. 返回\n";
	cout << "\n输入w继续游戏\n";

}
void TalkManager::talkScene_022() {
	cout << "\n";
	cout << "================================\n";
	cout << "              对话\n";
	cout << "================================\n";

	cout << "1. 副将\n";
	cout << "2. 返回\n";
	cout << "\n输入w继续游戏\n";

}
void TalkManager::talkScene_03() {
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

}
void TalkManager::talkScene_04() {
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

}


void TalkManager::talk_character_contnt_01(int current_character) {
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
void TalkManager::talk_character_contnt_020(int current_character) {

}
void TalkManager::talk_character_contnt_021(int current_character) {
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
void TalkManager::talk_character_contnt_022(int current_character) {
	switch (current_character) {
	case 1:
		//副将
		cout << "副将的对话没写" << "\n";
		break;

	}
}
void TalkManager::talk_character_contnt_03(int current_character) {
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
void TalkManager::talk_character_contnt_04(int current_character) {

}