#include "dataLoader.h"
#include "json.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>

namespace {

StatusEffect parseStatusEffect(const std::string& s) {
    if (s == "burn") return StatusEffect::Burn;
    if (s == "slow") return StatusEffect::Slow;
    if (s == "stun") return StatusEffect::Stun;
    if (s == "charge") return StatusEffect::Charge;
    return StatusEffect::None;
}

AttackScope parseScope(const std::string& s) {
    return (s == "all") ? AttackScope::All : AttackScope::Single;
}

StoryLine parseLine(const json::Value& v) {
    StoryLine line;
    line.text = v.has("text") ? v["text"].asString() : "";
    line.color = v.has("color") ? v["color"].asInt() : 14;
    line.sleep = v.has("sleep") ? v["sleep"].asInt() : 0;
    line.wait = v.has("wait") ? v["wait"].asBool() : false;
    return line;
}

std::vector<StoryLine> parseLines(const json::Value& v) {
    std::vector<StoryLine> lines;
    for (size_t i = 0; i < v.size(); i++) lines.push_back(parseLine(v[i]));
    return lines;
}

} // namespace

std::string DataLoader::readFileText(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("无法打开文件: " + path);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

SkillPool DataLoader::loadSkills(const std::string& path) {
    SkillPool pool;
    auto root = json::Value::parse(readFileText(path));
    for (const auto& id : root.keys()) {
        const auto& s = root[id];
        std::string type = s.has("type") ? s["type"].asString() : "damage";
        std::string name = s["name"].asString();
        std::string desc = s.has("description") ? s["description"].asString() : "";
        int cost = s.has("cost") ? s["cost"].asInt() : 0;
        AttackScope scope = s.has("scope") ? parseScope(s["scope"].asString()) : AttackScope::Single;

        if (type == "heal") {
            int healAmount = s["healAmount"].asInt();
            pool[id] = std::make_unique<HealSkill>(name, desc, cost, healAmount, scope);
        } else if (type == "charge") {
            float mult = s.has("multiplier") ? static_cast<float>(s["multiplier"].asDouble()) : 1.0f;
            int stat = s.has("targetStat") ? s["targetStat"].asInt() : -1;
            int dur = s.has("duration") ? s["duration"].asInt() : 1;
            pool[id] = std::make_unique<ChargingSkill>(name, desc, cost, mult, stat, dur, scope);
        } else {
            int power = s["power"].asInt();
            StatusEffect se = s.has("statusEffect") ? parseStatusEffect(s["statusEffect"].asString()) : StatusEffect::None;
            float hit = s.has("hitRate") ? static_cast<float>(s["hitRate"].asDouble()) : 0.95f;
            pool[id] = std::make_unique<DamageSkill>(name, desc, cost, power, scope, se, hit);
        }
    }
    return pool;
}

ItemPool DataLoader::loadItems(const std::string& path) {
    ItemPool pool;
    auto root = json::Value::parse(readFileText(path));
    for (const auto& id : root.keys()) {
        const auto& it = root[id];
        std::string name = it["name"].asString();
        std::string desc = it.has("description") ? it["description"].asString() : "";
        int price = it.has("price") ? it["price"].asInt() : 0;
        int healHP = it.has("healHP") ? it["healHP"].asInt() : 0;
        int healSP = it.has("healSP") ? it["healSP"].asInt() : 0;
        int statBonus = 0;
        int statIndex = -1;
        int duration = 0;
        if (it.has("buffStat") && it["buffStat"].isNumber()) statIndex = it["buffStat"].asInt();
        if (it.has("buffDuration")) duration = it["buffDuration"].asInt();
        if (it.has("buffMultiplier") && it["buffMultiplier"].asDouble() > 1.0) {
            statBonus = static_cast<int>((it["buffMultiplier"].asDouble() - 1.0) * 100.0);
        }
        pool[id] = std::make_unique<Consumable>(id, name, desc, price, healHP, healSP, statBonus, statIndex, duration);
    }
    return pool;
}

std::unique_ptr<Combatant> DataLoader::loadCombatant(const std::string& path,
                                                     const SkillPool& skillPool) {
    auto c = json::Value::parse(readFileText(path));
    std::string name = c["name"].asString();
    std::string id = c.has("id") ? c["id"].asString() : "";
    int level = c.has("level") ? c["level"].asInt() : 1;
    int hp = c["hp"].asInt();
    int sp = c["sp"].asInt();
    int exp = c.has("exp") ? c["exp"].asInt() : 0;

    int baseStats[4] = {0, 0, 0, 0};
    if (c.has("baseStats")) {
        const auto& bs = c["baseStats"];
        for (size_t i = 0; i < 4 && i < bs.size(); i++) baseStats[i] = bs[i].asInt();
    }

    std::vector<SkillBase*> skills;
    if (c.has("skills")) {
        const auto& sk = c["skills"];
        for (size_t i = 0; i < sk.size(); i++) {
            auto it = skillPool.find(sk[i].asString());
            if (it != skillPool.end()) skills.push_back(it->second.get());
        }
    }

    std::unordered_map<std::string, int> inventory;
    if (c.has("inventory")) {
        const auto& inv = c["inventory"];
        for (const auto& id : inv.keys()) inventory[id] = inv[id].asInt();
    }

    return std::make_unique<Combatant>(name, level, hp, sp, exp, baseStats, skills, inventory, id);
}

GameData DataLoader::loadGameData(const std::string& dataDir) {
    GameData data;
    data.skillPool = loadSkills(dataDir + "skill.json");
    // 道具未实现：data.itemPool 保持为空（实现后在此 loadItems(dataDir + "item.json")）
    return data;
}

std::vector<std::unique_ptr<Combatant>> DataLoader::loadPartyTemplates(const std::string& battlePath,
                                                                       const SkillPool& skillPool) {
    std::vector<std::unique_ptr<Combatant>> party;
    size_t slash = battlePath.find_last_of("/\\");
    std::string dir = (slash == std::string::npos) ? "" : battlePath.substr(0, slash + 1);

    auto root = json::Value::parse(readFileText(battlePath));
    party.push_back(loadCombatant(dir + root["player_ref"].asString(), skillPool));

    if (root.has("companions_ref")) {
        const auto& refs = root["companions_ref"];
        for (size_t i = 0; i < refs.size(); i++)
            party.push_back(loadCombatant(dir + refs[i].asString(), skillPool));
    } else if (root.has("companion_ref")) {
        party.push_back(loadCombatant(dir + root["companion_ref"].asString(), skillPool));
    }
    return party;
}

Battle DataLoader::loadBattle(const std::string& battlePath, const GameData& gameData) {
    Battle battle;

    // 以 battle json 所在目录作为数据根目录（用于定位敌方模板文件）
    size_t slash = battlePath.find_last_of("/\\");
    std::string dir = (slash == std::string::npos) ? "" : battlePath.substr(0, slash + 1);

    auto root = json::Value::parse(readFileText(battlePath));
    const auto& enemyRefs = root["enemies_ref"];
    for (size_t i = 0; i < enemyRefs.size(); i++)
        battle.enemies.push_back(loadCombatant(dir + enemyRefs[i].asString(), gameData.skillPool));

    battle.config.playerFirst = !root.has("first_side") || root["first_side"].asString() == "player";
    battle.config.disableRun = root.has("disable_run") && root["disable_run"].asBool();
    battle.config.disableItems = root.has("disable_items") && root["disable_items"].asBool();
    return battle;
}

StoryData DataLoader::loadStory(const std::string& path) {
    StoryData data;
    auto root = json::Value::parse(readFileText(path));
    const auto& scenes = root["scenes"];
    for (size_t i = 0; i < scenes.size(); i++) {
        const auto& s = scenes[i];
        Scene scene;
        scene.id = s["id"].asInt();
        if (s.has("lines")) scene.lines = parseLines(s["lines"]);
        if (s.has("choice")) {
            const auto& c = s["choice"];
            scene.choice.prompt = c.has("prompt") ? c["prompt"].asString() : "";
            const auto& opts = c["options"];
            for (size_t j = 0; j < opts.size(); j++) {
                const auto& o = opts[j];
                StoryOption opt;
                std::string key = o.has("key") ? o["key"].asString() : "a";
                opt.key = key.empty() ? 'a' : key[0];
                opt.text = o.has("text") ? o["text"].asString() : "";
                opt.branch = o.has("branch") ? o["branch"].asInt() : 0;
                if (o.has("lines")) opt.lines = parseLines(o["lines"]);
                scene.choice.options.push_back(opt);
            }
        }
        data.scenes.push_back(scene);
    }
    return data;
}
