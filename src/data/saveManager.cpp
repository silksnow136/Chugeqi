#include "saveManager.h"
#include "core/json.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <cstdio>

namespace {

// 转义 JSON 字符串中的特殊字符（引号/反斜杠/换行等）；中文 UTF-8 字节原样保留
std::string jsonEscape(const std::string& s) {
    std::string out;
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) { char buf[8]; std::snprintf(buf, sizeof(buf), "\\u%04x", c); out += buf; }
                else out += static_cast<char>(c);
        }
    }
    return out;
}

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

    json::Value root = json::Value::parse(text);
    info.hasSave = true;
    info.gold = root["gold"].asInt();
    info.sceneId = root["scene"].asInt();
    info.branchId = root["branch"].asInt();

    const json::Value& party = root["party"];
    if (party.size() > 0) {
        info.name = party[0]["name"].asString();
        info.level = party[0]["level"].asInt();
    }
    return info;
}

void SaveManager::save(int slot, const std::vector<Combatant*>& party,
                       const SkillPool& skillPool, const Meta& meta) const {
    if (!validSlot(slot)) return;

    std::string j;
    j += "{\n";
    j += "  \"scene\": " + std::to_string(meta.sceneId) + ",\n";
    j += "  \"branch\": " + std::to_string(meta.branchId) + ",\n";
    j += "  \"gold\": " + std::to_string(meta.gold) + ",\n";
    j += "  \"party\": [\n";

    for (size_t i = 0; i < party.size(); i++) {
        const Combatant* c = party[i];
        j += "    {\"id\": \"" + jsonEscape(c->getId()) + "\", ";
        j += "\"name\": \"" + jsonEscape(c->getName()) + "\", ";
        j += "\"level\": " + std::to_string(c->getLevel()) + ", ";
        j += "\"hp\": " + std::to_string(c->getHP()) + ", ";
        j += "\"sp\": " + std::to_string(c->getSP()) + ", ";
        j += "\"exp\": " + std::to_string(c->getExp()) + ", ";
        j += "\"str\": " + std::to_string(c->getBaseStat(0)) + ", ";
        j += "\"mag\": " + std::to_string(c->getBaseStat(1)) + ", ";
        j += "\"end\": " + std::to_string(c->getBaseStat(2)) + ", ";
        j += "\"agi\": " + std::to_string(c->getBaseStat(3)) + ", ";

        // 技能
        j += "\"skills\": [";
        bool first = true;
        for (auto* s : c->getSkills()) {
            std::string sid = findSkillId(skillPool, s);
            if (sid.empty()) continue;
            if (!first) j += ", ";
            j += "\"" + jsonEscape(sid) + "\"";
            first = false;
        }
        j += "], ";

        // 背包
        j += "\"items\": {";
        first = true;
        for (const auto& kv : c->getInventory()) {
            if (!first) j += ", ";
            j += "\"" + jsonEscape(kv.first) + "\": " + std::to_string(kv.second);
            first = false;
        }
        j += "}, ";

        // 装备槽位（只存非空槽）
        j += "\"equip\": {";
        first = true;
        for (int es = 0; es < 4; es++) {
            const std::string& itemId = c->getEquippedItemId(es);
            if (itemId.empty()) continue;
            if (!first) j += ", ";
            j += "\"" + std::to_string(es) + "\": \"" + jsonEscape(itemId) + "\"";
            first = false;
        }
        j += "}}";
        if (i + 1 < party.size()) j += ",";
        j += "\n";
    }

    j += "  ]\n";
    j += "}\n";

    std::filesystem::create_directories(dir_);
    std::ofstream out(path(slot), std::ios::binary);
    if (!out) throw std::runtime_error("无法写入存档: " + path(slot));
    out << j;
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

    const json::Value& party = root["party"];
    for (size_t i = 0; i < party.size(); i++) {
        const json::Value& c = party[i];
        std::string id = c["id"].asString();
        std::string name = c["name"].asString();
        int stats[4] = { c["str"].asInt(), c["mag"].asInt(), c["end"].asInt(), c["agi"].asInt() };

        auto combatant = std::make_unique<Combatant>(
            name, c["level"].asInt(), c["hp"].asInt(), c["sp"].asInt(), c["exp"].asInt(),
            stats, std::vector<SkillBase*>{}, std::unordered_map<std::string, int>{}, id);

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
                        int bonus[4];
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
