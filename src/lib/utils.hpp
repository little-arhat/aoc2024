#pragma once

#include <filesystem>
#include <fstream>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


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


template <typename Stream, typename Tuple, std::size_t... I>
void read_helper(Stream& s, Tuple& args, std::index_sequence<I...>) {
    ((s >> std::get<I>(args)), ...);
}

template <typename Tuple, typename... Actions, std::size_t... I>
void apply_actions(Tuple& args,
                   std::index_sequence<I...>,
                   Actions&&... actions) {
    (std::forward<Actions>(actions)(std::get<I>(args)), ...);
}

template <typename... Args, typename... Actions>
auto read_lines(std::ifstream& file, Actions&&... actions) -> void {
    static_assert(sizeof...(Args) == sizeof...(Actions),
                  "Number of arguments and actions must be the same.");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::tuple<Args...> args;
        read_helper(s, args, std::index_sequence_for<Args...>{});
        apply_actions(args,
                      std::index_sequence_for<Args...>{},
                      std::forward<Actions>(actions)...);
    }
}

/// todo: more flexible reading (e.g. read n-tuple, call lambda)
template <typename Action>
auto read_lines(std::ifstream& file, Action action) -> void {
    std::string line;
    while (std::getline(file, line)) {
        action(line);
    }
}

template <typename T>
auto sign(T val) -> int {
    return (T(0) < val) - (val < T(0));
}

template <typename T>
auto yn(T val) -> std::string {
    return val ? "yes" : "no";
}
