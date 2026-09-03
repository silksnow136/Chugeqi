#include "story.h"
#include "json.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

std::string readFileText(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("无法打开文件: " + path);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
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

StoryData loadStory(const std::string& path) {
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
