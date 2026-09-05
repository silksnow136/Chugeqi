#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <cstring>

namespace json {

// ---------------------------------------------------------------------------
// 轻量 JSON 解析器（递归下降，只读）。
//   支持：对象 / 数组 / 字符串 / 整数 / 浮点 / 布尔 / null。
//   用途：加载 data/*.json 静态模板（技能池、战斗配置、角色模板等）。
//   注意：\u 转义仅支持 BMP（最多 3 字节 UTF-8）；访问缺省键或越界下标会抛异常。
// ---------------------------------------------------------------------------
class Value {
public:
    enum class Type { Null, Bool, Int, Double, String, Array, Object };

    Value() : type_(Type::Null) {}
    Value(bool b) : type_(Type::Bool), boolVal_(b) {}
    Value(int i) : type_(Type::Int), intVal_(i), doubleVal_(static_cast<double>(i)) {}
    Value(long long i) : type_(Type::Int), intVal_(i), doubleVal_(static_cast<double>(i)) {}
    Value(double d) : type_(Type::Double), doubleVal_(d) {}
    Value(const char* s) : type_(Type::String), strVal_(s) {}
    Value(const std::string& s) : type_(Type::String), strVal_(s) {}

    Type type() const { return type_; }
    bool isNull() const { return type_ == Type::Null; }
    bool isObject() const { return type_ == Type::Object; }
    bool isArray() const { return type_ == Type::Array; }
    bool isString() const { return type_ == Type::String; }
    bool isNumber() const { return type_ == Type::Int || type_ == Type::Double; }
    bool isBool() const { return type_ == Type::Bool; }

    bool asBool() const { return boolVal_; }
    int asInt() const {
        if (type_ == Type::Int) return static_cast<int>(intVal_);
        if (type_ == Type::Double) return static_cast<int>(doubleVal_);
        return 0;
    }
    double asDouble() const {
        if (type_ == Type::Double) return doubleVal_;
        if (type_ == Type::Int) return static_cast<double>(intVal_);
        return 0.0;
    }
    const std::string& asString() const { return strVal_; }

    const Value& operator[](size_t i) const { return arr_.at(i); }
    const Value& operator[](const std::string& key) const { return obj_.at(key); }

    bool has(const std::string& key) const { return obj_.count(key) > 0; }
    size_t size() const {
        if (type_ == Type::Array) return arr_.size();
        if (type_ == Type::Object) return obj_.size();
        return 0;
    }

    std::vector<std::string> keys() const {
        std::vector<std::string> out;
        for (const auto& p : obj_) out.push_back(p.first);
        return out;
    }

    // 解析整段 JSON 文本；有多余内容或语法错误时抛出 runtime_error
    static Value parse(const std::string& text) {
        size_t pos = 0;
        Value v = parseValue(text, pos);
        skipWhitespace(text, pos);
        if (pos != text.size()) throw std::runtime_error("JSON: trailing data");
        return v;
    }

private:
    Type type_;
    bool boolVal_ = false;
    long long intVal_ = 0;
    double doubleVal_ = 0.0;
    std::string strVal_;
    std::vector<Value> arr_;
    std::map<std::string, Value> obj_;

    static void skipWhitespace(const std::string& s, size_t& pos) {
        while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) pos++;
    }

    // 按当前首字符分发到对应的解析函数
    static Value parseValue(const std::string& s, size_t& pos) {
        skipWhitespace(s, pos);
        if (pos >= s.size()) throw std::runtime_error("JSON: unexpected end");
        char c = s[pos];
        if (c == '{') return parseObject(s, pos);
        if (c == '[') return parseArray(s, pos);
        if (c == '"') return Value(parseString(s, pos));
        if (c == 't' || c == 'f') return parseBool(s, pos);
        if (c == 'n') { expect(s, pos, "null"); return Value(); }
        return parseNumber(s, pos);
    }

    static void expect(const std::string& s, size_t& pos, const char* lit) {
        size_t n = std::strlen(lit);
        if (s.compare(pos, n, lit) != 0) throw std::runtime_error("JSON: invalid literal");
        pos += n;
    }

    static Value parseBool(const std::string& s, size_t& pos) {
        if (s.compare(pos, 4, "true") == 0) { pos += 4; return Value(true); }
        if (s.compare(pos, 5, "false") == 0) { pos += 5; return Value(false); }
        throw std::runtime_error("JSON: invalid bool");
    }

    // 解析 { ... }；键值对存入 std::map（键有序，遍历结果稳定）
    static Value parseObject(const std::string& s, size_t& pos) {
        Value obj;
        obj.type_ = Type::Object;
        pos++; // '{'
        skipWhitespace(s, pos);
        if (pos < s.size() && s[pos] == '}') { pos++; return obj; }
        while (true) {
            skipWhitespace(s, pos);
            if (pos >= s.size() || s[pos] != '"') throw std::runtime_error("JSON: expected key");
            std::string key = parseString(s, pos);
            skipWhitespace(s, pos);
            if (pos >= s.size() || s[pos] != ':') throw std::runtime_error("JSON: expected ':'");
            pos++;
            Value v = parseValue(s, pos);
            obj.obj_[key] = v;
            skipWhitespace(s, pos);
            if (pos < s.size() && s[pos] == ',') { pos++; continue; }
            if (pos < s.size() && s[pos] == '}') { pos++; return obj; }
            throw std::runtime_error("JSON: expected ',' or '}'");
        }
    }

    // 解析 [ ... ]；元素按顺序存入 vector
    static Value parseArray(const std::string& s, size_t& pos) {
        Value arr;
        arr.type_ = Type::Array;
        pos++; // '['
        skipWhitespace(s, pos);
        if (pos < s.size() && s[pos] == ']') { pos++; return arr; }
        while (true) {
            Value v = parseValue(s, pos);
            arr.arr_.push_back(v);
            skipWhitespace(s, pos);
            if (pos < s.size() && s[pos] == ',') { pos++; continue; }
            if (pos < s.size() && s[pos] == ']') { pos++; return arr; }
            throw std::runtime_error("JSON: expected ',' or ']'");
        }
    }

    // 解析 " ... "：处理常见转义；\u 仅支持 BMP（代理对未处理）
    static std::string parseString(const std::string& s, size_t& pos) {
        pos++; // opening quote
        std::string out;
        while (pos < s.size()) {
            char c = s[pos++];
            if (c == '"') return out;
            if (c == '\\') {
                if (pos >= s.size()) throw std::runtime_error("JSON: bad escape");
                char e = s[pos++];
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case 'u': {
                        if (pos + 4 > s.size()) throw std::runtime_error("JSON: bad unicode");
                        unsigned cp = 0;
                        for (int i = 0; i < 4; i++) {
                            char h = s[pos++];
                            cp <<= 4;
                            if (h >= '0' && h <= '9') cp |= static_cast<unsigned>(h - '0');
                            else if (h >= 'a' && h <= 'f') cp |= static_cast<unsigned>(h - 'a' + 10);
                            else if (h >= 'A' && h <= 'F') cp |= static_cast<unsigned>(h - 'A' + 10);
                            else throw std::runtime_error("JSON: bad unicode hex");
                        }
                        if (cp < 0x80) {
                            out += static_cast<char>(cp);
                        } else if (cp < 0x800) {
                            out += static_cast<char>(0xC0 | (cp >> 6));
                            out += static_cast<char>(0x80 | (cp & 0x3F));
                        } else {
                            out += static_cast<char>(0xE0 | (cp >> 12));
                            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                            out += static_cast<char>(0x80 | (cp & 0x3F));
                        }
                        break;
                    }
                    default: throw std::runtime_error("JSON: bad escape char");
                }
            } else {
                out += c;
            }
        }
        throw std::runtime_error("JSON: unterminated string");
    }

    // 解析数字：含小数/指数时存 double，否则存 long long
    static Value parseNumber(const std::string& s, size_t& pos) {
        size_t start = pos;
        bool isDouble = false;
        if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) pos++;
        while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) pos++;
        if (pos < s.size() && s[pos] == '.') {
            isDouble = true; pos++;
            while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) pos++;
        }
        if (pos < s.size() && (s[pos] == 'e' || s[pos] == 'E')) {
            isDouble = true; pos++;
            if (pos < s.size() && (s[pos] == '-' || s[pos] == '+')) pos++;
            while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) pos++;
        }
        std::string num = s.substr(start, pos - start);
        if (isDouble) return Value(std::strtod(num.c_str(), nullptr));
        return Value(static_cast<long long>(std::strtoll(num.c_str(), nullptr, 10)));
    }
};

} // namespace json
