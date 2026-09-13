#include "saveManager.h"
#include "sqlite3.h"
#include <functional>
#include <stdexcept>

namespace {

// 绑定参数并执行一条 INSERT/DELETE 语句（不读取结果）
void runStatement(sqlite3* db, const char* sql, const std::function<void(sqlite3_stmt*)>& bind) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        throw std::runtime_error(err);
    }
    bind(stmt);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(err);
    }
    sqlite3_finalize(stmt);
}

// 由技能指针反查 skill_id
std::string findSkillId(const SkillPool& skillPool, const SkillBase* skill) {
    for (const auto& p : skillPool) {
        if (p.second.get() == skill) return p.first;
    }
    return "";
}

} // namespace

SaveManager::SaveManager(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::string err = db ? sqlite3_errmsg(db) : "无法打开数据库";
        if (db) sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error(err);
    }

    const char* schema =
        "CREATE TABLE IF NOT EXISTS characters ("
        "  id TEXT PRIMARY KEY, name TEXT NOT NULL, level INTEGER NOT NULL,"
        "  hp INTEGER NOT NULL, sp INTEGER NOT NULL, exp INTEGER NOT NULL,"
        "  str INTEGER NOT NULL, mag INTEGER NOT NULL, endur INTEGER NOT NULL, agi INTEGER NOT NULL);"
        "CREATE TABLE IF NOT EXISTS learned_skills ("
        "  character_id TEXT NOT NULL, slot INTEGER NOT NULL, skill_id TEXT NOT NULL,"
        "  PRIMARY KEY(character_id, slot));"
        "CREATE TABLE IF NOT EXISTS inventory ("
        "  character_id TEXT NOT NULL, item_id TEXT NOT NULL, count INTEGER NOT NULL,"
        "  PRIMARY KEY(character_id, item_id));";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, schema, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "建表失败";
        sqlite3_free(errMsg);
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error(err);
    }
}

SaveManager::~SaveManager() {
    if (db) sqlite3_close(db);
}

bool SaveManager::hasSave() const {
    sqlite3_stmt* stmt = nullptr;
    bool result = false;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM characters", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) result = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::unique_ptr<Combatant>> SaveManager::loadParty(const SkillPool& skillPool) const {
    std::vector<std::unique_ptr<Combatant>> party;
    std::unordered_map<std::string, Combatant*> byId;
    sqlite3_stmt* stmt = nullptr;

    // 1. 角色基础状态
    const char* sqlChar =
        "SELECT id, name, level, hp, sp, exp, str, mag, endur, agi FROM characters ORDER BY rowid";
    if (sqlite3_prepare_v2(db, sqlChar, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int level = sqlite3_column_int(stmt, 2);
            int hp = sqlite3_column_int(stmt, 3);
            int sp = sqlite3_column_int(stmt, 4);
            int exp = sqlite3_column_int(stmt, 5);
            int stats[4] = { sqlite3_column_int(stmt, 6), sqlite3_column_int(stmt, 7),
                             sqlite3_column_int(stmt, 8), sqlite3_column_int(stmt, 9) };
            auto c = std::make_unique<Combatant>(name, level, hp, sp, exp, stats,
                                                 std::vector<SkillBase*>{},
                                                 std::unordered_map<std::string, int>{}, id);
            byId[id] = c.get();
            party.push_back(std::move(c));
        }
        sqlite3_finalize(stmt);
    }

    // 2. 持有技能
    const char* sqlSkill = "SELECT character_id, skill_id FROM learned_skills ORDER BY character_id, slot";
    if (sqlite3_prepare_v2(db, sqlSkill, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string cid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string skillId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            auto cIt = byId.find(cid);
            auto sIt = skillPool.find(skillId);
            if (cIt != byId.end() && sIt != skillPool.end()) cIt->second->addSkill(sIt->second.get());
        }
        sqlite3_finalize(stmt);
    }

    // 3. 持有道具
    const char* sqlItem = "SELECT character_id, item_id, count FROM inventory";
    if (sqlite3_prepare_v2(db, sqlItem, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string cid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string itemId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int count = sqlite3_column_int(stmt, 2);
            auto cIt = byId.find(cid);
            if (cIt != byId.end()) cIt->second->addItem(itemId, count);
        }
        sqlite3_finalize(stmt);
    }

    return party;
}

void SaveManager::saveParty(const std::vector<std::unique_ptr<Combatant>>& party,
                            const SkillPool& skillPool) const {
    const char* insertChar =
        "INSERT INTO characters (id, name, level, hp, sp, exp, str, mag, endur, agi)"
        " VALUES (?,?,?,?,?,?,?,?,?,?)";
    const char* insertSkill = "INSERT INTO learned_skills (character_id, slot, skill_id) VALUES (?,?,?)";
    const char* insertItem = "INSERT INTO inventory (character_id, item_id, count) VALUES (?,?,?)";

    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM characters", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM learned_skills", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM inventory", nullptr, nullptr, nullptr);

    for (const auto& c : party) {
        runStatement(db, insertChar, [&](sqlite3_stmt* stmt) {
            sqlite3_bind_text(stmt, 1, c->getId().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, c->getName().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 3, c->getLevel());
            sqlite3_bind_int(stmt, 4, c->getHP());
            sqlite3_bind_int(stmt, 5, c->getSP());
            sqlite3_bind_int(stmt, 6, c->getExp());
            for (int i = 0; i < 4; i++) sqlite3_bind_int(stmt, 7 + i, c->getEffectiveStat(i));
        });

        int slot = 0;
        for (auto* s : c->getSkills()) {
            std::string skillId = findSkillId(skillPool, s);
            if (skillId.empty()) continue;
            runStatement(db, insertSkill, [&](sqlite3_stmt* stmt) {
                sqlite3_bind_text(stmt, 1, c->getId().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 2, slot);
                sqlite3_bind_text(stmt, 3, skillId.c_str(), -1, SQLITE_TRANSIENT);
            });
            slot++;
        }

        for (const auto& kv : c->getInventory()) {
            runStatement(db, insertItem, [&](sqlite3_stmt* stmt) {
                sqlite3_bind_text(stmt, 1, c->getId().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, kv.first.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 3, kv.second);
            });
        }
    }

    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
}

void SaveManager::resetSave() const {
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM characters", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM learned_skills", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DELETE FROM inventory", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
}
