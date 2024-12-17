#pragma once

#include <charconv>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>


auto slurp(const std::string& filename) -> std::string {
    std::ifstream file(filename);
    std::string s((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
    return s;
}


auto aoc(int argc, char* argv[], std::string_view def) -> std::string {
    if (argc > 1) {
        return {argv[1]};
    } else {
        return std::string{def};
    }
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


template <typename... Args, typename... Actions>
auto read_lines(const std::string& filename, Actions&&... actions) -> void {
    std::ifstream file(filename);
    read_lines<Args...>(file, std::forward<Actions>(actions)...);
}


/// todo: more flexible reading (e.g. read n-tuple, call lambda, sliptchar)
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


template <typename T>
auto split(const std::string_view& input, char c) -> std::vector<T> {
    return std::views::split(input, c) | std::views::transform([](auto&& s) {
               T value;
               std::from_chars(&*s.begin(), &*s.end(), value);
               return value;
           }) |
           std::ranges::to<std::vector<int>>();
}

template <typename T>
auto to_pair(const std::string& input, char c) -> std::pair<T, T> {
    auto pos = input.find(c);
    T first, second;
    std::from_chars(input.data(), input.data() + pos, first);
    std::from_chars(input.data() + pos + 1,
                    input.data() + input.size(),
                    second);
    return {first, second};
}

template <typename T>
auto split_gen(const std::string_view& inp, char c) {
    return inp | std::views::split(c) | std::views::transform([](auto&& part) {
               T value;
               std::from_chars(&*part.begin(), &*part.end(), value);
               return value;
           });
}


template <typename Container>
std::ostream& print_container(std::ostream& os,
                              const Container& c,
                              const char* sep = ",") {
    os << '{';
    for (auto it = c.begin(); it != c.end(); ++it)
        os << (it == c.begin() ? "" : sep) << *it;
    return os << '}';
}

template <typename K,
          typename V,
          template <typename, typename, typename...>
          class Map>
std::ostream& print_map(std::ostream& os,
                        const Map<K, V>& m,
                        const char* sep = ", ") {
    os << '{';
    for (auto it = m.begin(); it != m.end(); ++it)
        os << (it == m.begin() ? "" : sep) << it->first << ':' << it->second;
    return os << '}';
}


template <typename T, typename Y>
constexpr bool operator==(const std::pair<T, Y>& lhs,
                          const std::pair<T, Y>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}


struct pair_hash {
    template <class T1, class T2>
    auto operator()(const std::pair<T1, T2>& p) const -> std::size_t {
        return std::hash<T1>()(p.first) ^ (std::hash<T2>()(p.second) << 1);
    }
};


struct tuple_hash {
    template <typename... Args>
    auto operator()(const std::tuple<Args...>& t) const -> std::size_t {
        return std::apply(
            [](const auto&... elems) {
                std::size_t seed = 0;
                ((seed ^= std::hash<std::decay_t<decltype(elems)>>{}(elems) +
                          0x9e3779b9 + (seed << 6) + (seed >> 2)),
                 ...);
                return seed;
            },
            t);
    }
};


template <typename T>
auto deep_copy(const std::vector<std::vector<T>>& original)
    -> std::vector<std::vector<T>> {
    return original;
}


auto digit(char c) -> int {
    return c - '0';
}


using pi = std::pair<int, int>;


constexpr pi operator+(const pi& lhs, const pi& rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}


constexpr pi operator-(const pi& lhs, const pi& rhs) {
    return {lhs.first - rhs.first, lhs.second - rhs.second};
}


auto wrap(int v, int bound) -> int {
    auto s = sign(v);
    auto vc = std::abs(v) % bound;
    return s > 0 || vc == 0 ? vc : bound - vc;
}
