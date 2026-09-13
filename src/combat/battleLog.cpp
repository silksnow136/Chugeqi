#include "battleLog.h"

void BattleLog::add(const std::string& msg) {
    lines_.push_back(msg);
    while (lines_.size() > MAX_LINES) {
        lines_.pop_front();
    }
}

const std::deque<std::string>& BattleLog::lines() const {
    return lines_;
}
