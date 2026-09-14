#include<iostream>
#include"SceneManager.h"
#include"Game.h"
#include "TalkManager.h"
#include "PharManager.h"
#include "SceneMap.h"
#include "core/console.h"
#include <cctype>
#include <limits>
#include <memory>

SceneManager::SceneManager(Game& game): game(game)
{
	current_scene_id = 0;//剧情初步存档,显示当前场景id

	autoPlay = false;//判断是否自动播放剧情

	// 启动时载入剧情数据（仅一次）
	scenes = DataLoader::loadStory("data/story/story.json").scenes;

	talkManager.setSceneManager(this);
}
SceneManager::~SceneManager() = default;

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
	console::enterRaw(); // 对话停顿期间持有 raw：关闭回显、吞掉按键
	if (autoPlay) {
		string tip = "  【按ESC手动对话，按q加速当前对话】";
		cout << tip;
		console::sleep(1200);//1200ms后播放下一条剧情
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
		string tip = "  【按ESC自动对话，按其他键继续对话，按q加速当前对话】";
		cout << tip;
		int key;
		key=console::readKey(); // 读取键盘按键，但不是显示键盘输入
		if (key == 27) {
			changeAuto();
		}
		deleteWords(tip);
	}
	
	console::drainInput(); // 停顿结束吞残留按键并恢复 canonical
}

int SceneManager::showScene_id() {
	return current_scene_id;
}

int SceneManager::showBranch_id() {
	return current_branch_id;
}

const string& SceneManager::getAdvancePrompt(int scene_id) const {
	static const string empty;
	for (const auto& s : scenes) {
		if (s.id == scene_id) return s.advance;
	}
	return empty;
}

void SceneManager::changeScene(int scene_id)
{
	// 离开旧场景
	current_scene_id = scene_id;
	// 重置分支
	current_branch_id = 0;
}

//场景功能显示管理 —— 使用网格地图 + WASD 移动交互
void SceneManager::showSceneManager(int scene_id, int branch_id) {
	// 运行网格地图交互（走向 NPC 触发对话/药店/战斗，ESC 退出）
	SceneMap::runSceneMap(game, *this, scene_id, branch_id);
}

// 读档恢复：跳过幕次剧情，直接进入指定幕的地图
void SceneManager::resumeScene(int scene_id, int branch_id) {
	current_scene_id = scene_id;
	current_branch_id = branch_id;
	showSceneManager(scene_id, branch_id);
}


// 进入药店系统（供地图移动交互调用）
void SceneManager::enterPharmacy(Game& game1) {
	if (pharManager == nullptr) {
		pharManager = std::make_unique<PharManager>(
			game1.getItemPool(),
			game1.getGold()
		);
	}
	// 清理可能残留的输入缓冲区
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	pharManager->phar(game1.getPlayer());
}

//输出剧情对话,逐字打印，Sleep1控制逐字打印速度
void SceneManager::printWords(string tips,int color,int sleep,int sleep1) {
	console::enterRaw(); // 播放期间持有 raw：关闭回显、吞掉玩家按键
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
		//按q\Q加速剧情
		if (console::kbhit()) {
			int key;
			key = console::readKey(); // 读取键盘按键，但不是显示键盘输入
			if (key == 113 || key == 81) {
				sleep1=1;
			}
		}
		cout << flush;
		console::sleep(sleep1);
	}

	cout << "\n";
	console::sleep(sleep);
	console::drainInput(); // 播放结束吞残留按键并恢复 canonical
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
	if (!playLines(scene->lines)) {
		return;
	}

	// 分支选择（第二幕）
	if (!scene->choice.options.empty()) {

		if (!playChoice(*scene, branch_id)) {
			return;
		}

		current_branch_id = branch_id; // 记录当前分支，供存档

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

	current_scene_id = scene_id;

	if (scene_id == 2) {
		showSceneManager(current_scene_id, branch_id);
	} else {
		showSceneManager(current_scene_id);
	}
}

bool SceneManager::playLines(const std::vector<StoryLine>& lines)
{
	for (const auto& line : lines) {

		// 播放剧情
		printWords(line.text, line.color, line.sleep);

		// 等待玩家继续
		if (line.wait) {
			nextLine();
		}

		// 当前剧情要求进入战斗
		if (line.battle) {

			bool victory = startStoryBattle(line.battleId);

			// 战斗失败，不继续播放后面的剧情
			if (!victory) {
				return false;
			}
		}
	}

	return true;
}

bool SceneManager::playChoice(const Scene& scene, int& branch_id)
{
	if (scene.choice.options.empty())
		return true;

	char choice;
	bool choice_test = true;

	do {
		printWords(scene.choice.prompt, 11, 0);

		cout << "\n>";
		cin >> choice;

		bool matched = false;

		for (const auto& opt : scene.choice.options) {

			if (std::tolower(choice) == std::tolower(opt.key)) {

				bool success = playLines(opt.lines);

				if (!success) {
					return false;
				}

				branch_id = opt.branch;
				choice_test = false;
				matched = true;

				break;
			}
		}

		if (!matched) {
			printWords(
				"未知分支，请重新选择！！！\n",
				11,
				0
			);
		}

	} while (choice_test);

	return true;
}


bool SceneManager::startStoryBattle(const std::string& battleId)
{
	if (battleId.empty()) {
		return true;
	}

	std::string battlePath = "data/battles/battle_" + battleId + ".json";

	try {
		Battle battle = DataLoader::loadBattle(
			battlePath,
			game.getGameData()
		);

		Combatant* player = &game.getPlayer();

		std::vector<Combatant*> companions;
		if (Combatant* c = game.getCompanion(); c != nullptr && c->isAlive()) {
			companions.push_back(c);
		}

		std::vector<Combatant*> enemies;

		for (auto& enemy : battle.enemies) {
			enemies.push_back(enemy.get());
		}

		CombatSystem combat(
			player,
			companions,
			enemies,
			battle.config,
				&game.getGameData().itemPool
		);

		return combat.startBattle();
	}
	catch (const std::exception& e) {
		std::cerr << "\n战斗加载失败：" << e.what() << "\n";
		std::cout << "按任意键返回。\n";
		console::readKey();
		return false;
	}
}