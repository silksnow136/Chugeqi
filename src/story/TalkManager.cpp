#include<iostream>
#include"Game.h"
#include "TalkManager.h"
#include "SceneManager.h"
#include "core/console.h"
#include "core/json.h"

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
			talkCharacter(1,num, branch_id);
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
				talkCharacter(2,num, branch_id);
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
				talkCharacter(2,num, branch_id);
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
			talkCharacter(3,num, branch_id);
		}
	}
	else {
		sceneManager->scene = sceneManager->handleCommand(game1, sceneCommand1);
	}
}

// 第四幕
void TalkManager::talkScene04(Game& game1, int branch_id)
{
	talkScene_04();

	string sceneCommand1;

	cout << "\n> ";
	cin >> sceneCommand1;

	if (sceneCommand1 == "1" ||
		sceneCommand1 == "2" ||
		sceneCommand1 == "3" ||
		sceneCommand1 == "4" ||
		sceneCommand1 == "5" ||
		sceneCommand1 == "6")
	{
		int num = stoi(sceneCommand1);

		if (num == 6)
		{
			// 返回大世界
			sceneManager->setSceneState(
				SceneManager::SceneState::ORIGIN_SCENE
			);
		}
		else
		{
			// 播放第四幕对应人物对话
			talkCharacter(4,num, branch_id);
		}
	}
	else
	{
		sceneManager->scene =
			sceneManager->handleCommand(game1, sceneCommand1);
	}
}

void TalkManager::waitForReturn() {
	string tip1 = "输入任意按键返回";
	cout << tip1;
	console::pause();
	sceneManager->deleteWords(tip1);

}

//对话播放
void TalkManager::talkCharacter(int scene_id,int character_id, int branch_id) {

	// 刷新场景
	sceneManager->refreshScene(branch_id);
	// 设置当前人物
	sceneManager->setCurrentCharacter(character_id);
	// 播放人物对话
	// 从 talk.json 读取
	loadDialogue(scene_id,branch_id,character_id);

	waitForReturn();
}

// 外部直接播放对话（不刷新场景地图）
void TalkManager::talkCharacterExternal(int scene_id, int character_id, int branch_id) {
	console::setColor(10);
	std::cout << "\n===== 对话 =====" << std::endl;
	console::setColor(7);
	sceneManager->setCurrentCharacter(character_id);
	loadDialogue(scene_id, branch_id, character_id);
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

//播放一句话
void TalkManager::playDialogue(string speaker, string text)
{	
	
	string words = speaker + "：" + text;

	sceneManager->printWords(words,14,800,80);
	sceneManager->nextLine();
	console::setColor(14);
}
//从json中读取对话
void TalkManager::loadDialogue(int scene_id, int branch_id, int character_id)
{
	try
	{
		// 读取 talk.json
		string text = DataLoader::readFileText("data/talk.json");

		// 使用 JSON 解析器
		json::Value root = json::Value::parse(text);

		// 获取 scenes
		const auto& scenes = root["scenes"];

		// =========================
		// 第一步：寻找 scene_id
		// =========================

		for (size_t i = 0; i < scenes.size(); i++)
		{
			const auto& scene = scenes[i];

			if (scene["scene_id"].asInt() != scene_id)
			{
				continue;
			}

			// =========================
			// 第二幕：需要 branch_id
			// =========================

			if (scene_id == 2)
			{
				if (!scene.has("branches"))
				{
					cout << "第二幕没有找到 branches！" << endl;
					return;
				}

				const auto& branches = scene["branches"];

				for (size_t j = 0; j < branches.size(); j++)
				{
					const auto& branch = branches[j];

					if (branch["branch_id"].asInt() != branch_id)
					{
						continue;
					}

					if (!branch.has("characters"))
					{
						cout << "没有找到人物数据！" << endl;
						return;
					}

					const auto& characters = branch["characters"];

					// =========================
					// 寻找人物
					// =========================

					for (size_t k = 0; k < characters.size(); k++)
					{
						const auto& character = characters[k];

						if (character["character_id"].asInt() != character_id)
						{
							continue;
						}

						const auto& dialogues = character["dialogue"];

						// =========================
						// 播放所有对话
						// =========================

						for (size_t l = 0; l < dialogues.size(); l++)
						{
							const auto& dialogue = dialogues[l];

							string speaker = dialogue["speaker"].asString();
							string text = dialogue["text"].asString();

							playDialogue(speaker, text);
						}

						return;
					}

					cout << "没有找到对应人物的对话！" << endl;
					return;
				}

				cout << "没有找到对应的 branch_id！" << endl;
				return;
			}

			// =========================
			// 第一、三、四幕
			// =========================

			if (!scene.has("characters"))
			{
				cout << "当前场景没有找到人物数据！" << endl;
				return;
			}

			const auto& characters = scene["characters"];

			// =========================
			// 寻找人物
			// =========================

			for (size_t j = 0; j < characters.size(); j++)
			{
				const auto& character = characters[j];

				if (character["character_id"].asInt() != character_id)
				{
					continue;
				}

				const auto& dialogues = character["dialogue"];

				// =========================
				// 播放所有对话
				// =========================

				for (size_t k = 0; k < dialogues.size(); k++)
				{
					const auto& dialogue = dialogues[k];

					string speaker = dialogue["speaker"].asString();
					string text = dialogue["text"].asString();

					playDialogue(speaker, text);
				}

				return;
			}

			cout << "没有找到对应人物的对话！" << endl;
			return;
		}

		cout << "没有找到对应的 scene_id！" << endl;
	}
	catch (const std::exception& e)
	{
		cout << "读取 talk.json 失败！" << endl;
		cout << e.what() << endl;
	}
}