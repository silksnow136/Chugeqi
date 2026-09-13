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
		doLoad(1); // 继续征途：默认读取存档位 1（可用 load 1 / load 2 指定）
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
	else if (command == "save" || command == "load") {
		cout << "用法：" << command << " 1 或 " << command << " 2" << "\n";
	}
	else if (command.rfind("save ", 0) == 0) {
		int slot = atoi(command.c_str() + 5);
		if (SaveManager::validSlot(slot)) doSave(slot);
		else cout << "无效存档位，请输入 1 或 2。" << "\n";
	}
	else if (command.rfind("load ", 0) == 0) {
		int slot = atoi(command.c_str() + 5);
		if (SaveManager::validSlot(slot)) doLoad(slot);
		else cout << "无效存档位，请输入 1 或 2。" << "\n";
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
		<< "  " << "save 1/2" << "      " << "存档到指定存档位" << "\n"
		<< "  " << "load 1/2" << "      " << "读取指定存档位" << "\n"
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

void Game::doSave(int slot)
{
	try {
		SaveManager save("save.db");

		// 队伍（当前仅主角；同伴由战斗系统另行管理）
		std::vector<Combatant*> party{ player.get() };
		save.saveParty(slot, party, gameData.skillPool);

		// 元信息：剧情进度 / 分支 / 金币
		SaveManager::Meta meta;
		meta.sceneId = sceneManager.showScene_id();
		meta.branchId = sceneManager.showBranch_id();
		meta.gold = gold;
		save.saveMeta(slot, meta);

		cout << "已保存到存档位 " << slot << "（场景 " << meta.sceneId << "）。" << "\n";
	}
	catch (const std::exception& e) {
		cout << "存档失败：" << e.what() << "\n";
	}
}

void Game::doLoad(int slot)
{
	try {
		SaveManager save("save.db");
		if (!save.hasSave(slot)) {
			cout << "存档位 " << slot << " 为空。" << "\n";
			return;
		}

		auto party = save.loadParty(slot, gameData.skillPool, gameData.itemPool);
		if (party.empty()) {
			cout << "存档位 " << slot << " 无角色数据。" << "\n";
			return;
		}
		player = std::move(party[0]); // 主角

		SaveManager::Meta meta = save.loadMeta(slot);
		gold = meta.gold;
		scene_id = meta.sceneId;

		cout << "已读取存档位 " << slot << "。" << "\n";
		sceneManager.ShowBackground(scene_id);
	}
	catch (const std::exception& e) {
		cout << "读档失败：" << e.what() << "\n";
	}
}