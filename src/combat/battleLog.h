#pragma once
#include <deque>
#include <string>

// 独立战斗日志模块：供测试时记录并显示战斗过程。
// 最多保留 MAX_LINES 条消息，新的消息追加在末尾（最下面一行是最新消息）。
class BattleLog {
public:
    static const size_t MAX_LINES = 7;

    void add(const std::string& msg); // 追加一条消息，超出上限时丢弃最旧的一条
    const std::deque<std::string>& lines() const;

private:
    std::deque<std::string> lines_;
};
