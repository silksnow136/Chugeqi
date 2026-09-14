#include<iostream>
#include "TalkManager.h"
#include "SceneManager.h"
#include "core/console.h"
#include "core/json.h"

namespace {
// 缓存 talk.json 解析结果（只读一次，避免每次对话都重新读文件+解析）
const json::Value& talkRoot() {
	static json::Value root = json::Value::parse(DataLoader::readFileText("data/story/talk.json"));
	return root;
}
}

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
bool TalkManager::talkCharacterExternal(int scene_id, const string& name, int branch_id) {
	if (!loadDialogue(scene_id, branch_id, name)) return false;
	waitForReturn();
	return true;
}

// 简单对话：talk.json 的 "simple" 表（name → 一行文本）
void TalkManager::playSimpleTalk(const string& name) {
	string text;
	try {
		const json::Value& root = talkRoot();
		if (root.has("simple") && root["simple"].has(name))
			text = root["simple"][name].asString();
	} catch (...) {}

	console::setColor(10);
	std::cout << "\n===== " << name << " =====" << std::endl;
	console::setColor(7);
	std::cout << (text.empty() ? "（此处无可对话内容）" : text) << std::endl;
	console::pause();
}

//播放一句话
void TalkManager::playDialogue(string speaker, string text)
{
	string words = speaker + "：" + text;

	sceneManager->printWords(words,14,600,80);
	sceneManager->nextLine();
	console::setColor(14);
}

// 从 json 读取对话：按 scene_id -> (branch_id) -> character_name 匹配
bool TalkManager::loadDialogue(int scene_id, int branch_id, const string& name)
{
	try
	{
		const json::Value& root = talkRoot();
		const auto& scenes = root["scenes"];

		for (size_t i = 0; i < scenes.size(); i++)
		{
			const auto& scene = scenes[i];
			if (scene["scene_id"].asInt() != scene_id) continue;

			// 第二幕：需要 branch_id
			if (scene_id == 2)
			{
				if (!scene.has("branches")) return false;
				const auto& branches = scene["branches"];
				for (size_t j = 0; j < branches.size(); j++)
				{
					const auto& branch = branches[j];
					if (branch["branch_id"].asInt() != branch_id) continue;
					if (!branch.has("characters")) return false;
					const auto& characters = branch["characters"];
					for (size_t k = 0; k < characters.size(); k++)
					{
						const auto& character = characters[k];
						if (character["character_name"].asString() != name) continue;
						const auto& dialogues = character["dialogue"];
						console::setColor(10);
						std::cout << "\n===== 对话 =====" << std::endl;
						console::setColor(7);
						for (size_t l = 0; l < dialogues.size(); l++)
							playDialogue(dialogues[l]["speaker"].asString(), dialogues[l]["text"].asString());
						return true;
					}
				}
				return false;
			}

			// 第一、三、四幕
			if (!scene.has("characters")) return false;
			const auto& characters = scene["characters"];
			for (size_t j = 0; j < characters.size(); j++)
			{
				const auto& character = characters[j];
				if (character["character_name"].asString() != name) continue;
				const auto& dialogues = character["dialogue"];
				console::setColor(10);
				std::cout << "\n===== 对话 =====" << std::endl;
				console::setColor(7);
				for (size_t k = 0; k < dialogues.size(); k++)
					playDialogue(dialogues[k]["speaker"].asString(), dialogues[k]["text"].asString());
				return true;
			}
			return false;
		}
	}
	catch (const std::exception&) {}
	return false;
}
