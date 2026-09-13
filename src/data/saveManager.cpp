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
        "CREATE TABLE IF NOT EXISTS meta ("
        "  slot_id INTEGER PRIMARY KEY,"
        "  scene_id INTEGER NOT NULL, branch_id INTEGER NOT NULL, gold INTEGER NOT NULL);"
        "CREATE TABLE IF NOT EXISTS characters ("
        "  slot_id INTEGER NOT NULL, id TEXT NOT NULL, name TEXT NOT NULL,"
        "  level INTEGER NOT NULL, hp INTEGER NOT NULL, sp INTEGER NOT NULL, exp INTEGER NOT NULL,"
        "  str INTEGER NOT NULL, mag INTEGER NOT NULL, endur INTEGER NOT NULL, agi INTEGER NOT NULL,"
        "  PRIMARY KEY(slot_id, id));"
        "CREATE TABLE IF NOT EXISTS learned_skills ("
        "  slot_id INTEGER NOT NULL, character_id TEXT NOT NULL, slot INTEGER NOT NULL, skill_id TEXT NOT NULL,"
        "  PRIMARY KEY(slot_id, character_id, slot));"
        "CREATE TABLE IF NOT EXISTS inventory ("
        "  slot_id INTEGER NOT NULL, character_id TEXT NOT NULL, item_id TEXT NOT NULL, count INTEGER NOT NULL,"
        "  PRIMARY KEY(slot_id, character_id, item_id));"
        "CREATE TABLE IF NOT EXISTS equipment ("
        "  slot_id INTEGER NOT NULL, character_id TEXT NOT NULL, equip_slot INTEGER NOT NULL, item_id TEXT NOT NULL,"
        "  PRIMARY KEY(slot_id, character_id, equip_slot));";

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

bool SaveManager::hasSave(int slot) const {
    if (!validSlot(slot)) return false;
    sqlite3_stmt* stmt = nullptr;
    bool result = false;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM characters WHERE slot_id = ?", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
        if (sqlite3_step(stmt) == SQLITE_ROW) result = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::unique_ptr<Combatant>> SaveManager::loadParty(int slot,
                                                               const SkillPool& skillPool,
                                                               const ItemPool& itemPool) const {
    std::vector<std::unique_ptr<Combatant>> party;
    if (!validSlot(slot)) return party;
    std::unordered_map<std::string, Combatant*> byId;
    sqlite3_stmt* stmt = nullptr;

    // 1. 角色基础状态（str/mag/endur/agi 存的是基础属性，不含装备加成）
    const char* sqlChar =
        "SELECT id, name, level, hp, sp, exp, str, mag, endur, agi FROM characters"
        " WHERE slot_id = ? ORDER BY rowid";
    if (sqlite3_prepare_v2(db, sqlChar, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
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
    const char* sqlSkill = "SELECT character_id, skill_id FROM learned_skills WHERE slot_id = ? ORDER BY character_id, slot";
    if (sqlite3_prepare_v2(db, sqlSkill, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
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
    const char* sqlItem = "SELECT character_id, item_id, count FROM inventory WHERE slot_id = ?";
    if (sqlite3_prepare_v2(db, sqlItem, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string cid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string itemId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int count = sqlite3_column_int(stmt, 2);
            auto cIt = byId.find(cid);
            if (cIt != byId.end()) cIt->second->addItem(itemId, count);
        }
        sqlite3_finalize(stmt);
    }

    // 4. 装备槽位：按物品池查加成并重新装配
    const char* sqlEquip = "SELECT character_id, equip_slot, item_id FROM equipment WHERE slot_id = ?";
    if (sqlite3_prepare_v2(db, sqlEquip, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string cid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int equipSlot = sqlite3_column_int(stmt, 1);
            std::string itemId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            auto cIt = byId.find(cid);
            auto it = itemPool.find(itemId);
            if (cIt != byId.end() && it != itemPool.end()) {
                const Equipment* eq = dynamic_cast<const Equipment*>(it->second.get());
                if (eq) {
                    int bonus[4];
                    eq->getStatBonus(bonus);
                    cIt->second->equipItem(equipSlot, itemId, bonus);
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    return party;
}

void SaveManager::saveParty(int slot, const std::vector<Combatant*>& party,
                            const SkillPool& skillPool) const {
    if (!validSlot(slot)) return;

    const char* insertChar =
        "INSERT INTO characters (slot_id, id, name, level, hp, sp, exp, str, mag, endur, agi)"
        " VALUES (?,?,?,?,?,?,?,?,?,?,?)";
    const char* insertSkill = "INSERT INTO learned_skills (slot_id, character_id, slot, skill_id) VALUES (?,?,?,?)";
    const char* insertItem = "INSERT INTO inventory (slot_id, character_id, item_id, count) VALUES (?,?,?,?)";
    const char* insertEquip = "INSERT INTO equipment (slot_id, character_id, equip_slot, item_id) VALUES (?,?,?,?)";

    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    // 删除该存档位的旧记录（sqlite3_exec 不支持绑定参数，手动 prepare 绑定）
    {
        sqlite3_stmt* d = nullptr;
        const char* dels[] = {
            "DELETE FROM characters WHERE slot_id = ?",
            "DELETE FROM learned_skills WHERE slot_id = ?",
            "DELETE FROM inventory WHERE slot_id = ?",
            "DELETE FROM equipment WHERE slot_id = ?",
        };
        for (const char* sql : dels) {
            if (sqlite3_prepare_v2(db, sql, -1, &d, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(d, 1, slot);
                sqlite3_step(d);
                sqlite3_finalize(d);
            }
        }
    }

    for (const auto& c : party) {
        runStatement(db, insertChar, [&](sqlite3_stmt* stmt) {
            sqlite3_bind_int(stmt, 1, slot);
            sqlite3_bind_text(stmt, 2, c->getId().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, c->getName().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 4, c->getLevel());
            sqlite3_bind_int(stmt, 5, c->getHP());
            sqlite3_bind_int(stmt, 6, c->getSP());
            sqlite3_bind_int(stmt, 7, c->getExp());
            // 存基础属性（不含装备加成），装备由 equipment 表单独保存
            for (int i = 0; i < 4; i++) sqlite3_bind_int(stmt, 8 + i, c->getBaseStat(i));
        });

        int skillSlot = 0;
        for (auto* s : c->getSkills()) {
            std::string skillId = findSkillId(skillPool, s);
            if (skillId.empty()) continue;
            runStatement(db, insertSkill, [&](sqlite3_stmt* stmt) {
                sqlite3_bind_int(stmt, 1, slot);
                sqlite3_bind_text(stmt, 2, c->getId().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 3, skillSlot);
                sqlite3_bind_text(stmt, 4, skillId.c_str(), -1, SQLITE_TRANSIENT);
            });
            skillSlot++;
        }

        for (const auto& kv : c->getInventory()) {
            runStatement(db, insertItem, [&](sqlite3_stmt* stmt) {
                sqlite3_bind_int(stmt, 1, slot);
                sqlite3_bind_text(stmt, 2, c->getId().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 3, kv.first.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 4, kv.second);
            });
        }

        for (int es = 0; es < 4; es++) {
            const std::string& itemId = c->getEquippedItemId(es);
            if (itemId.empty()) continue;
            runStatement(db, insertEquip, [&](sqlite3_stmt* stmt) {
                sqlite3_bind_int(stmt, 1, slot);
                sqlite3_bind_text(stmt, 2, c->getId().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 3, es);
                sqlite3_bind_text(stmt, 4, itemId.c_str(), -1, SQLITE_TRANSIENT);
            });
        }
    }

    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
}

void SaveManager::saveParty(int slot, const std::vector<std::unique_ptr<Combatant>>& party,
                            const SkillPool& skillPool) const {
    std::vector<Combatant*> ptrs;
    ptrs.reserve(party.size());
    for (const auto& p : party) ptrs.push_back(p.get());
    saveParty(slot, ptrs, skillPool);
}

void SaveManager::saveMeta(int slot, const Meta& meta) const {
    if (!validSlot(slot)) return;
    runStatement(db,
        "INSERT INTO meta (slot_id, scene_id, branch_id, gold) VALUES (?,?,?,?)"
        " ON CONFLICT(slot_id) DO UPDATE SET scene_id=excluded.scene_id,"
        " branch_id=excluded.branch_id, gold=excluded.gold",
        [&](sqlite3_stmt* stmt) {
            sqlite3_bind_int(stmt, 1, slot);
            sqlite3_bind_int(stmt, 2, meta.sceneId);
            sqlite3_bind_int(stmt, 3, meta.branchId);
            sqlite3_bind_int(stmt, 4, meta.gold);
        });
}

SaveManager::Meta SaveManager::loadMeta(int slot) const {
    Meta meta;
    if (!validSlot(slot)) return meta;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, "SELECT scene_id, branch_id, gold FROM meta WHERE slot_id = ?", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, slot);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            meta.sceneId = sqlite3_column_int(stmt, 0);
            meta.branchId = sqlite3_column_int(stmt, 1);
            meta.gold = sqlite3_column_int(stmt, 2);
        }
    }
    sqlite3_finalize(stmt);
    return meta;
}

void SaveManager::resetSave(int slot) const {
    if (!validSlot(slot)) return;
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    sqlite3_stmt* d = nullptr;
    const char* dels[] = {
        "DELETE FROM meta WHERE slot_id = ?",
        "DELETE FROM characters WHERE slot_id = ?",
        "DELETE FROM learned_skills WHERE slot_id = ?",
        "DELETE FROM inventory WHERE slot_id = ?",
        "DELETE FROM equipment WHERE slot_id = ?",
    };
    for (const char* sql : dels) {
        if (sqlite3_prepare_v2(db, sql, -1, &d, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(d, 1, slot);
            sqlite3_step(d);
            sqlite3_finalize(d);
        }
    }
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
}
