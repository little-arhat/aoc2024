#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include <print>
#include <string>


std::vector<std::string> slurp(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("File does not exist: " + filename);
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}


template <typename M, typename K, typename V>
auto get_or_default(M&& m, K&& k, V def) -> V {
    auto it = m.find(k);
    if (it != m.end()) {
        return it->second;
    }

    return def;
}
