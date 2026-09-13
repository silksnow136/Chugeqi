#include "Game.h"
#include<iostream>
#include "map.h"
#include"SceneManager.h"
#include "core/console.h"
#include "data/saveManager.h"
#include <cstdlib>

Game::Game(): sceneManager(*this){}//创建 SceneManager 的时候，把当前这个 Game 对象传给它

void Game::run() {
	console::init();//初始化控制台编码（UTF-8）
	initialize();
	gameLoop();
}

void Game::initialize() {
	gameData = DataLoader::loadGameData("data/");

	player = DataLoader::loadCombatant(
		"data/player.json",
		gameData.skillPool
	);

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
		saveMenu(); // 继续征途：打开存读档界面，选择存档位读档
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
		console::setColor(3);
		map();
		console::setColor(14);
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
	console::setColor(4);
	cout << "========================================" << "\n"
		<< "       楚歌起--霸王之陨，长乐未央    " << "\n"
		<< "========================================" << "\n";
	console::setColor(4);
	cout<< "     力拔山兮气盖世，破釜沉舟破强秦。" << "\n"
		<< "     乌江耻渡千秋义，霸业虽忘骨亦雄。" << "\n";
	console::setColor(14);
	cout << "\n";
	cout << "1.开始游戏" << "   " << "2.继续征途" << "   " << "3.退出游戏" << "   " << "4.帮助";
	
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
		<< "  " << "q\Q " << "        " << "加速当前对话剧情" << "\n"
		
		<< "===========================" << "\n";
}

Combatant& Game::getPlayer()
{
	return *player;
}

ItemPool& Game::getItemPool()
{
	return gameData.itemPool;
}

int& Game::getGold()
{
	return gold;
}


GameData& Game::getGameData()
{
	return gameData;
}
void Game::doSave(int slot)
{
	try {
		SaveManager save("saves");

		// 队伍（当前仅主角；同伴由战斗系统另行管理）
		std::vector<Combatant*> party{ player.get() };

		// 元信息：剧情进度 / 分支 / 金币
		SaveManager::Meta meta;
		meta.sceneId = sceneManager.showScene_id();
		meta.branchId = sceneManager.showBranch_id();
		meta.gold = gold;

		save.save(slot, party, gameData.skillPool, meta);

		cout << "已保存到存档位 " << slot << "（场景 " << meta.sceneId << "）。" << "\n";
	}
	catch (const std::exception& e) {
		cout << "存档失败：" << e.what() << "\n";
	}
}

void Game::doLoad(int slot)
{
	try {
		SaveManager save("saves");
		if (!save.hasSave(slot)) {
			cout << "存档位 " << slot << " 为空。" << "\n";
			return;
		}

		auto data = save.load(slot, gameData.skillPool, gameData.itemPool);
		if (data.party.empty()) {
			cout << "存档位 " << slot << " 无角色数据。" << "\n";
			return;
		}
		player = std::move(data.party[0]); // 主角

		gold = data.meta.gold;
		scene_id = data.meta.sceneId;

		cout << "已读取存档位 " << slot << "。" << "\n";
		sceneManager.ShowBackground(scene_id);
	}
	catch (const std::exception& e) {
		cout << "读档失败：" << e.what() << "\n";
	}
}

// 显示单个存档位信息（供存读档界面复用）
static void printSlotLine(const SaveManager::SlotInfo& info) {
	if (!info.hasSave) {
		cout << "（空）";
	} else {
		cout << "Lv." << info.level << " " << info.name
			 << "  金钱:" << info.gold
			 << "  场景:" << info.sceneId;
		if (info.branchId != 0) cout << "-" << info.branchId;
	}
	cout << "\n";
}

// 存档位子菜单：1 存档(覆盖需确认) / 2 读档 / 0 返回（单键即时响应）
// 返回 true 表示发生了读档（游戏继续，需退出整个存读档界面）
bool Game::slotMenu(int slot) {
	SaveManager save("saves");
	while (true) {
		console::clearScreen();
		console::setColor(14);
		cout << "========== 存档位 " << slot << " ==========" << "\n";
		console::setColor(7);
		auto info = save.getSlotInfo(slot);
		printSlotLine(info);
		console::setColor(14);
		cout << "-----------------------------------" << "\n";
		console::setColor(7);
		cout << "[1]存档  [2]读档  [0]返回" << "\n";

		int key = console::readKey();
		if (key == '0') return false;

		if (key == '1') { // 存档
			if (info.hasSave) {
				// 覆盖确认：y/n，非法输入重新询问
				while (true) {
					console::setColor(14);
					cout << "\n要覆盖存档" << slot << "吗？[y/n]" << "\n";
					console::setColor(7);
					int yn = console::readKey();
					if (yn == 'y' || yn == 'Y') {
						doSave(slot);
						console::pause();
						return false;
					}
					if (yn == 'n' || yn == 'N') {
						break; // 取消覆盖，返回子菜单
					}
					console::setColor(12);
					cout << "\n无效输入，请按 y/n。" << "\n";
					console::setColor(7);
					console::pause();
				}
			} else {
				doSave(slot);
				console::pause();
				return false;
			}
		}
		else if (key == '2') { // 读档
			if (!info.hasSave) {
				console::setColor(12);
				cout << "\n存档位 " << slot << " 为空。" << "\n";
				console::setColor(7);
				console::pause();
			} else {
				doLoad(slot);
				return true;
			}
		}
		else {
			console::setColor(12);
			cout << "\n无效输入，请按 0/1/2。" << "\n";
			console::setColor(7);
			console::pause();
		}
	}
}

bool Game::saveMenu() {
	SaveManager save("saves");
	while (true) {
		console::clearScreen();
		console::setColor(14);
		cout << "============== 存读档 ==============" << "\n";
		console::setColor(7);
		for (int s = 1; s <= SaveManager::SLOT_COUNT; s++) {
			cout << "存档位 " << s << "：";
			printSlotLine(save.getSlotInfo(s));
		}
		console::setColor(14);
		cout << "-----------------------------------" << "\n";
		console::setColor(7);
		cout << "0. 返回上级" << "\n";
		cout << "1. 存/读/覆盖 存档位 1" << "\n";
		cout << "2. 存/读/覆盖 存档位 2" << "\n";

		int key = console::readKey();
		if (key == '0') return false;
		if (key == '1' || key == '2') {
			if (slotMenu(key - '0')) return true; // 读档后游戏继续，退出界面
		} else {
			console::setColor(12);
			cout << "\n无效输入，请按 0/1/2。" << "\n";
			console::setColor(7);
			console::pause();
		}
	}

}