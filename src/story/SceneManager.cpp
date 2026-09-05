#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include"map.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "ForgeManager.h"
#include "BackGround.h"
#include "console.h"
#include <cctype>

SceneManager::SceneManager(Game& game): game(game)
{
	current_scene_id = 0;//剧情初步存档,显示当前场景id
	current_state = SceneState::ORIGIN_SCENE;
	current_character = 0;

	autoPlay = false;//判断是否自动播放剧情
	unique_map_print = false;

	// 启动时载入剧情数据（仅一次）
	scenes = loadStory("data/story.json").scenes;

	talkManager.setSceneManager(this);
}

bool SceneManager::current_Auto() {
	return autoPlay;
}

void SceneManager::changeAuto() {
	autoPlay = !autoPlay;
}

void SceneManager::deleteWords(string tip) {
	// 以下是让提示【按任意键继续对话】消失
	for (int i = 0; i < tip.length(); i++) {
		cout << '\b'; // \b是退格符，循环提示长度的次数，使光标到达提示之前
	}
	// 用空格覆盖所有残留字符，再退回到行首
	cout << string(tip.length(), ' ');
	for (int i = 0; i < tip.length(); i++) {
		cout << '\b';
	}
}
void SceneManager::nextLine() {
	if (autoPlay) {
		string tip = "  【按ESC手动对话】";
		cout << tip;
		console::sleep(1500);//1500ms后播放下一条剧情
		deleteWords(tip);
		//console::kbhit()非阻塞检测按键输入
		if (console::kbhit()) {
			int key;
			key = console::readKey(); // 读取键盘按键，但不是显示键盘输入
			if (key == 27) {
				changeAuto();
			}
		}
	}
	else {
		string tip = "  【按ESC自动对话，按其他键继续对话】";
		cout << tip;
		int key;
		key=console::readKey(); // 读取键盘按键，但不是显示键盘输入
		if (key == 27) {
			changeAuto();
		}
		deleteWords(tip);
	}
	
}

int SceneManager::showScene_id() {
	return current_scene_id;
}

void SceneManager::changeScene(int scene_id)
{
	// 离开旧场景
	current_scene_id = scene_id;
	// 进入新场景时重置状态
	current_state = SceneState::ORIGIN_SCENE;
	// 重置对话人物
	current_character = 0;
	// 允许新场景重新打印地图
	unique_map_print = false;
}

//场景功能显示管理
void SceneManager::showSceneManager(int scene_id, int branch_id) {
	switch (scene_id) {
	case 1:
		map_Manager(scene_id, branch_id);
		console::setColor(14);
		sceneManager(game, branch_id);
		break;

	case  2:
		map_Manager(scene_id, branch_id);
		console::setColor(14);
		sceneManager(game, branch_id);
		break;

	case 3:
		map_Manager(scene_id, branch_id);
		console::setColor(14);
		sceneManager(game, branch_id);
		break;

	case 4:
		map_Manager(scene_id, branch_id);
		console::setColor(14);
		sceneManager(game, branch_id);
		break;

	default:
		break;
	}
}


void choiceList_01() {
	cout << "1. 对话\n";
	cout << "2. 药房\n";
	cout << "3. 锻造\n";
	cout << "\n输入w继续游戏\n";
}

void SceneManager::refreshScene(int branch_id) {
	//清屏
	console::clearScreen();
	map_Manager(showScene_id(), branch_id);
}

//判断并执行命令
bool SceneManager::handleCommand(Game& game1, const string& sceneCommand)
{	
	bool ch = true;
	// 判断是否为需要退出当前场景的指令
	if (sceneCommand == "south" || sceneCommand == "w" ||  sceneCommand == "W" ||  
		sceneCommand == "north" || sceneCommand == "n" || sceneCommand == "N" ||
		sceneCommand == "quit" || sceneCommand == "start")
	{	
		// 结束当前 SceneManager
		ch = false;
	}

	// 清屏
	console::clearScreen();
	game1.gameCommand(sceneCommand);
	// 执行完普通指令后，等待玩家按键
	
	if (sceneCommand != "start") {
		cout << "输入任意按键返回";
		console::readKey();
		deleteWords("输入任意按键返回");
	}
	
	return ch;
}


// 显示当前场景背景
void SceneManager::showCurrentBackground() {
	switch (showScene_id()) {
	case 1:
		backGround_01();
		break;
	case 2:
		backGround_02();
		break;
	case 3:
		backGround_03();
		break;
	case 4:
		backGround_04();
		break;
	}
}
//场景功能管理,1对话系统+命令系统；2药店系统；3锻造系统
void SceneManager::sceneManager(Game& game1, int branch_id) {
	string sceneCommand;
	scene = true;
	while (scene) {
		switch (current_state) {

		//大世界场景
		case SceneState::ORIGIN_SCENE:

			refreshScene(branch_id);

			showCurrentBackground();
			
			choiceList_01();

			cout << "\n> ";
			cin >> sceneCommand;
			if (sceneCommand == "1" || sceneCommand == "2" || sceneCommand == "3") {
				int num = stoi(sceneCommand);//将string转化为int
				switch (num) {
				case 1:
					//进入对话
					current_state = SceneState::TALK;
					break;

				case 2:
					//进入药店
					current_state = SceneState::PHARMACY;
					//药店未制作！！！！！！！！！！！！！！！！！！！！！
					//不要在这里添加，这里只负责进入药店系统
					break;

				case 3:
					// 进入锻造
					current_state = SceneState::FORGE;
					//锻造未制作！！！！！！！！！！！！！！！！
					//不要在这里添加，这里只负责进入锻造系统
					break;
				}
			}
			else {
				scene = handleCommand(game1, sceneCommand);
			}
			break;

		// 选择对话人物
		case SceneState::TALK:
			
			refreshScene(branch_id);

			switch (showScene_id()) {
				//第一幕
			case 1:
				talkManager.talkScene01(game1, branch_id);
				break;

				//第二幕
			case 2:
				talkManager.talkScene02(game1, branch_id);
				break;

				//第三幕
			case 3:
				talkManager.talkScene03(game1, branch_id);
				break;

				//第四幕!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//第四幕对话暂时没有地方插入，第四幕项羽死了不知道如何合理的进行对话
			case 4:
				
				break;
			}
			
			break;

			//药房
		case SceneState::PHARMACY:

			refreshScene(branch_id);

			void phar();
			cout << "输入任意数字返回。\n";
			cin >> sceneCommand;
			//别忘记加入if/else输入指令
			current_state = SceneState::ORIGIN_SCENE;

			break;

			//锻造
		case SceneState::FORGE:

			refreshScene(branch_id);

			void forge();
			cout << "输入任意数字返回。\n";
			cin >> sceneCommand;
			//别忘记加入if/else输入指令
			current_state = SceneState::ORIGIN_SCENE;

			break;
		}
	}
}

//SceneManager::SceneState函数的返回值类型，后面是类的成员函数
//读取当前场景状态
SceneManager::SceneState SceneManager::getSceneState() const
{
	return current_state;
}
//进入新场景状态，便于从主世界进入对话,便于切换场景
void SceneManager::setSceneState(SceneState state)
{
	current_state = state;
}
//读取对话人物
int SceneManager::getCurrentCharacter() const
{
	return current_character;
}
//更改对话人物
void SceneManager::setCurrentCharacter(int character_id)
{
	current_character = character_id;
}

//输出剧情对话,逐字打印，Sleep1控制逐字打印速度
void SceneManager::printWords(string tips,int color,int sleep,int sleep1) {
	console::setColor(color);
	for (size_t i = 0; i < tips.length();)
	{
		unsigned char c = tips[i];
		// ASCII 字符
		if (c < 128)
		{
			cout << tips[i];
			i++;
		}
		// UTF-8 中文字符，通常占 3 个字节
		else
		{
			cout << tips.substr(i, 3);
			i += 3;
		}
		cout << flush;
		console::sleep(sleep1);
	}

	cout << "\n";
	console::sleep(sleep);
}
void SceneManager::ShowBackground(int scene_id = 0) {
	int key1;
	int branch_id = 0;
	string command = "quit";//控制游戏结束后结束循环退出游戏

	// 找到对应场景
	const Scene* scene = nullptr;
	for (const auto& s : scenes) {
		if (s.id == scene_id) { scene = &s; break; }
	}
	if (!scene) return;

	//清屏
	console::clearScreen();

	// 播放叙事
	playLines(scene->lines);

	// 分支选择（第二幕）
	if (!scene->choice.options.empty()) {
		playChoice(*scene, branch_id);
		console::sleep(2000);
	}

	// 第四幕：结束游戏
	if (scene_id == 4) {
		cout << "游戏结束，感谢您的游玩" << "\n"
			<< "请输入任意键退出游戏" << "\n";

		key1 = console::readKey();
		game.gameCommand(command);
		return;
	}

	cout << "输入任意按键继续";

	key1 = console::readKey();

	//清屏
	console::clearScreen();

	cout << "已存档" << "\n";
	current_scene_id = scene_id;
	cout << "输入w或south继续剧情" << "\n";

	if (scene_id == 2) {
		showSceneManager(current_scene_id, branch_id);
	} else {
		showSceneManager(current_scene_id);
	}
}

void SceneManager::playLines(const std::vector<StoryLine>& lines) {
	for (const auto& line : lines) {
		printWords(line.text, line.color, line.sleep);
		if (line.wait) nextLine();
	}
}

void SceneManager::playChoice(const Scene& scene, int& branch_id) {
	if (scene.choice.options.empty()) return;
	char choice;
	bool choice_test = true;
	do {
		printWords(scene.choice.prompt, 11, 0);
		cout << "\n" << ">";
		cin >> choice;
		bool matched = false;
		for (const auto& opt : scene.choice.options) {
			if (std::tolower(choice) == std::tolower(opt.key)) {
				playLines(opt.lines);
				branch_id = opt.branch;
				choice_test = false;
				matched = true;
				break;
			}
		}
		if (!matched) {
			printWords("未知分支，请重新选择！！！\n", 11, 0);
		}
	} while (choice_test);
}
