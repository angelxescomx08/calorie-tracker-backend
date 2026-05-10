#pragma once
#include <string>
#include <unordered_map>
#include <fstream>

namespace infrastructure {

class EnvLoader {
public:
    static std::unordered_map<std::string, std::string> load(const std::string& path = ".env") {
        std::unordered_map<std::string, std::string> vars;
        std::ifstream file(path);
        if (!file.is_open()) return vars;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = trim(line.substr(0, eq));
            std::string val = trim(line.substr(eq + 1));

            if (!val.empty() && val.size() >= 2) {
                char q = val.front();
                if ((q == '"' || q == '\'') && val.back() == q)
                    val = val.substr(1, val.size() - 2);
            }

            if (!key.empty()) vars[key] = val;
        }
        return vars;
    }

    static std::string get(const std::unordered_map<std::string, std::string>& vars,
                           const std::string& key,
                           const std::string& defaultVal = "") {
        auto it = vars.find(key);
        return it != vars.end() ? it->second : defaultVal;
    }

    static int getInt(const std::unordered_map<std::string, std::string>& vars,
                      const std::string& key,
                      int defaultVal = 0) {
        auto it = vars.find(key);
        if (it == vars.end() || it->second.empty()) return defaultVal;
        try { return std::stoi(it->second); } catch (...) { return defaultVal; }
    }

private:
    static std::string trim(std::string s) {
        auto start = s.find_first_not_of(" \t\r\n");
        auto end   = s.find_last_not_of(" \t\r\n");
        return start == std::string::npos ? "" : s.substr(start, end - start + 1);
    }
};

} // namespace infrastructure
