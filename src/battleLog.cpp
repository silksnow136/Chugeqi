#include "battleLog.h"

namespace {
    // 在指定宽度内按字符数居中（未区分全角/半角宽度，仅用于空日志提示）
    std::string center(const std::string& text, size_t width) {
        if (text.size() >= width) return text;
        size_t pad = (width - text.size()) / 2;
        return std::string(pad, ' ') + text;
    }
}

void BattleLog::add(const std::string& msg) {
    lines_.push_back(msg);
    while (lines_.size() > MAX_LINES) {
        lines_.pop_front();
    }
}

void BattleLog::clear() {
    lines_.clear();
}

const std::deque<std::string>& BattleLog::lines() const {
    return lines_;
}

bool BattleLog::empty() const {
    return lines_.empty();
}

std::string BattleLog::render() const {
    const size_t WIDTH = 40; // 空日志时“(暂无)”的居中参考宽度
    std::string out;
    for (size_t i = 0; i < MAX_LINES; i++) {
        if (i < lines_.size()) {
            out += lines_[i];
        } else if (i == MAX_LINES / 2 && empty()) {
            out += center("(暂无)", WIDTH);
        }
        out += '\n';
    }
    return out;
}
