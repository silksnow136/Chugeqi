#include<iostream>
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

void TalkManager::waitForReturn() {
	string tip1 = "输入任意按键返回";
	cout << tip1;
	console::pause();
	sceneManager->deleteWords(tip1);

}

// 外部直接播放对话（不刷新场景地图）
void TalkManager::talkCharacterExternal(int scene_id, int character_id, int branch_id) {
	console::setColor(10);
	std::cout << "\n===== 对话 =====" << std::endl;
	console::setColor(7);
	loadDialogue(scene_id, branch_id, character_id);
	waitForReturn();
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
