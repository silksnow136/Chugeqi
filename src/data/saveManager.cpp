#include "saveManager.h"
#include "core/json.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <exception>

namespace {

// 由技能指针反查 skill_id
std::string findSkillId(const SkillPool& skillPool, const SkillBase* skill) {
    for (const auto& p : skillPool) {
        if (p.second.get() == skill) return p.first;
    }
    return "";
}

// 读取整个文件为字符串；失败返回空串
std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// 从存档根节点恢复支线任务进度（旧存档无 quest 字段时保留默认值）
void parseQuest(const json::Value& root, QuestState& q) {
    if (!root.has("quest")) return;
    const json::Value& j = root["quest"];
    if (j.has("morale")) q.morale = j["morale"].asInt();
    if (j.has("q1")) q.q1 = j["q1"].asInt();
    if (j.has("q2")) q.q2 = j["q2"].asInt();
    if (j.has("q3")) q.q3 = j["q3"].asInt();
    if (j.has("q3choice")) q.q3choice = j["q3choice"].asInt();
    if (j.has("deserters")) q.deserters = j["deserters"].asInt();
    if (j.has("deserterTalked")) {
        const json::Value& a = j["deserterTalked"];
        for (size_t i = 0; i < 3 && i < a.size(); i++) q.deserterTalked[i] = a[i].asBool();
    }
    if (j.has("yinlingUnlocked")) q.yinlingUnlocked = j["yinlingUnlocked"].asBool();
    if (j.has("guanyingDefeated")) q.guanyingDefeated = j["guanyingDefeated"].asBool();
    if (j.has("raincoatWarned")) q.raincoatWarned = j["raincoatWarned"].asBool();
}

} // namespace

SaveManager::SaveManager(const std::string& saveDir) : dir_(saveDir) {
    if (!dir_.empty() && dir_.back() != '/' && dir_.back() != '\\') dir_ += '/';
}

std::string SaveManager::path(int slot) const {
    return dir_ + "save_" + std::to_string(slot) + ".json";
}

bool SaveManager::hasSave(int slot) const {
    if (!validSlot(slot)) return false;
    std::ifstream in(path(slot));
    return in.good();
}

SaveManager::SlotInfo SaveManager::getSlotInfo(int slot) const {
    SlotInfo info;
    if (!validSlot(slot)) return info;
    std::string text = readFile(path(slot));
    if (text.empty()) return info;

    try {
        json::Value root = json::Value::parse(text);
        info.gold = root["gold"].asInt();
        info.sceneId = root["scene"].asInt();
        info.branchId = root["branch"].asInt();

        const json::Value& party = root["party"];
        if (party.size() > 0) {
            info.name = party[0]["name"].asString();
            info.level = party[0]["level"].asInt();
        }
        info.hasSave = true; // 全部字段读取成功后才标记为有效存档
    } catch (const std::exception&) {
        // 存档损坏或字段缺失：按空档位处理（允许覆盖），避免异常上抛导致崩溃
        return info;
    }
    return info;
}

void SaveManager::save(int slot, const std::vector<Combatant*>& party,
                       const SkillPool& skillPool, const Meta& meta) const {
    if (!validSlot(slot)) return;

    json::Value root;
    root.set("scene", meta.sceneId);
    root.set("branch", meta.branchId);
    root.set("gold", meta.gold);

    const QuestState& q = meta.quest;
    json::Value quest = json::Value::object();
    quest.set("morale", q.morale);
    quest.set("q1", q.q1);
    quest.set("q2", q.q2);
    quest.set("q3", q.q3);
    quest.set("q3choice", q.q3choice);
    quest.set("deserters", q.deserters);
    json::Value talked = json::Value::array();
    for (int i = 0; i < 3; i++) talked.push(q.deserterTalked[i]);
    quest.set("deserterTalked", talked);
    quest.set("yinlingUnlocked", q.yinlingUnlocked);
    quest.set("guanyingDefeated", q.guanyingDefeated);
    quest.set("raincoatWarned", q.raincoatWarned);
    root.set("quest", quest);

    json::Value world = json::Value::object();
    world.set("mapName", meta.world.mapName);
    world.set("playerRow", meta.world.playerRow);
    world.set("playerCol", meta.world.playerCol);
    json::Value cleared = json::Value::object();
    for (const auto& kv : meta.world.cleared) {
        json::Value arr = json::Value::array();
        for (int code : kv.second) arr.push(code);
        cleared.set(kv.first, arr);
    }
    world.set("cleared", cleared);
    root.set("world", world);

    json::Value partyArr = json::Value::array();
    for (const Combatant* c : party) {
        json::Value obj = json::Value::object();
        obj.set("id", c->getId());
        obj.set("name", c->getName());
        obj.set("level", c->getLevel());
        obj.set("hp", c->getHP());
        obj.set("sp", c->getSP());
        obj.set("maxHp", c->getMaxHP());
        obj.set("maxSp", c->getMaxSP());
        obj.set("exp", c->getExp());
        obj.set("str", c->getBaseStat(0));
        obj.set("end", c->getBaseStat(1));
        obj.set("agi", c->getBaseStat(2));

        json::Value skills = json::Value::array();
        for (auto* s : c->getSkills()) {
            std::string sid = findSkillId(skillPool, s);
            if (sid.empty()) continue;
            skills.push(sid);
        }
        obj.set("skills", skills);

        json::Value items = json::Value::object();
        for (const auto& kv : c->getInventory()) {
            items.set(kv.first, kv.second);
        }
        obj.set("items", items);

        json::Value equip = json::Value::object();
        for (int es = 0; es < 2; es++) {
            const std::string& itemId = c->getEquippedItemId(es);
            if (itemId.empty()) continue;
            equip.set(std::to_string(es), itemId);
        }
        obj.set("equip", equip);

        partyArr.push(obj);
    }
    root.set("party", partyArr);

    std::filesystem::create_directories(dir_);
    std::ofstream out(path(slot), std::ios::binary);
    if (!out) throw std::runtime_error("无法写入存档: " + path(slot));
    out << root.stringify();
}

void SaveManager::save(int slot, const std::vector<std::unique_ptr<Combatant>>& party,
                       const SkillPool& skillPool, const Meta& meta) const {
    std::vector<Combatant*> ptrs;
    ptrs.reserve(party.size());
    for (const auto& p : party) ptrs.push_back(p.get());
    save(slot, ptrs, skillPool, meta);
}

SaveManager::SaveData SaveManager::load(int slot, const SkillPool& skillPool,
                                        const ItemPool& itemPool) const {
    SaveData data;
    if (!validSlot(slot)) return data;

    std::string text = readFile(path(slot));
    if (text.empty()) return data;

    json::Value root = json::Value::parse(text);
    data.meta.sceneId = root["scene"].asInt();
    data.meta.branchId = root["branch"].asInt();
    data.meta.gold = root["gold"].asInt();
    parseQuest(root, data.meta.quest);
    if (root.has("world")) {
        const json::Value& w = root["world"];
        if (w.has("mapName")) data.meta.world.mapName = w["mapName"].asString();
        if (w.has("playerRow")) data.meta.world.playerRow = w["playerRow"].asInt();
        if (w.has("playerCol")) data.meta.world.playerCol = w["playerCol"].asInt();
        if (w.has("cleared")) {
            const json::Value& cleared = w["cleared"];
            for (const auto& key : cleared.keys()) {
                const json::Value& arr = cleared[key];
                for (size_t i = 0; i < arr.size(); i++) data.meta.world.cleared[key].push_back(arr[i].asInt());
            }
        }
    }

    const json::Value& party = root["party"];
    for (size_t i = 0; i < party.size(); i++) {
        const json::Value& c = party[i];
        std::string id = c["id"].asString();
        std::string name = c["name"].asString();
        int stats[3] = { c["str"].asInt(), c["end"].asInt(), c["agi"].asInt() };
        int maxHp = c.has("maxHp") ? c["maxHp"].asInt() : -1;
        int maxSp = c.has("maxSp") ? c["maxSp"].asInt() : -1;

        auto combatant = std::make_unique<Combatant>(
            name, c["level"].asInt(), c["hp"].asInt(), c["sp"].asInt(), c["exp"].asInt(),
            stats, std::vector<SkillBase*>{}, std::unordered_map<std::string, int>{}, id, maxHp, maxSp);

        if (c.has("skills")) {
            const json::Value& sk = c["skills"];
            for (size_t k = 0; k < sk.size(); k++) {
                auto it = skillPool.find(sk[k].asString());
                if (it != skillPool.end()) combatant->addSkill(it->second.get());
            }
        }
        if (c.has("items")) {
            const json::Value& items = c["items"];
            for (const auto& key : items.keys()) combatant->addItem(key, items[key].asInt());
        }
        if (c.has("equip")) {
            const json::Value& equip = c["equip"];
            for (const auto& key : equip.keys()) {
                int es = std::stoi(key);
                std::string itemId = equip[key].asString();
                auto it = itemPool.find(itemId);
                if (it != itemPool.end()) {
                    const Equipment* eq = dynamic_cast<const Equipment*>(it->second.get());
                    if (eq) {
                        int bonus[3];
                        eq->getStatBonus(bonus);
                        combatant->equipItem(es, itemId, bonus);
                    }
                }
            }
        }
        data.party.push_back(std::move(combatant));
    }
    return data;
}

void SaveManager::resetSave(int slot) const {
    if (!validSlot(slot)) return;
    std::error_code ec;
    std::filesystem::remove(path(slot), ec);
}
