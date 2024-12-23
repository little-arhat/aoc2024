
#include <algorithm>
#include "utils.hpp"


struct array_cmp {
    bool operator()(const std::array<std::string, 3>& lhs,
                    const std::array<std::string, 3>& rhs) const {
        return lhs < rhs;
    }
};


template <typename T>
auto st3(T a, T b, T c) -> std::array<T, 3> {
    std::array<T, 3> arr = {a, b, c};
    std::sort(arr.begin(), arr.end());
    return arr;
}

auto chief(const std::string& s) -> bool {
    return s[0] == 't';
}


auto second(std::string s) {
    std::unordered_map<std::string, std::unordered_set<std::string>> v;
    read_lines(s, [&v](std::string line) {
        auto pos = line.find('-');
        auto f = line.substr(0, pos);
        auto t = line.substr(pos + 1);
        v[f].insert(t);
        // v[f].insert(f);  // simplify later
        v[t].insert(f);
        // v[t].insert(t);
    });

    std::vector<std::unordered_set<std::string>> parties;
    std::print("{{");
    for (auto& [el, ns] : v) {
        std::println("'{}': '{}',", el, join_str(ns, ','));

        std::unordered_set<std::string> party;
        for (auto& [o, os] : v) {
            if (o != el) {
                std::set_intersection(ns.begin(),
                                      ns.end(),
                                      os.begin(),
                                      os.end(),
                                      std::inserter(party, party.begin()));
            }
        }
        parties.push_back(party);
    }
    std::println("}}");

    auto best = parties[0];
    for (auto& p : parties) {
        std::println("{}: {}", p.size(), join_str(p, ','));
        if (p.size() > best.size()) {
            best = p;
        }
    }

    std::println("{}", join_str(best, ','));
}


auto first(std::string s) {
    std::unordered_map<std::string, std::unordered_set<std::string>> v;
    std::vector<std::pair<std::string, std::string>> e;
    read_lines(s, [&v, &e](std::string line) {
        auto pos = line.find('-');
        auto f = line.substr(0, pos);
        auto t = line.substr(pos + 1);
        e.push_back({f, t});
        v[f].insert(t);
        v[t].insert(f);
    });

    std::unordered_set<std::array<std::string, 3>, array_hash> triangles;
    for (auto [f, t] : e) {
        if (!chief(f) && !chief(t)) {
            continue;
        }
        for (auto fn : v[f]) {
            if (fn != t && v[fn].contains(t)) {
                triangles.insert(st3(f, t, fn));
            }
        }
        for (auto tn : v[t]) {
            if (tn != f && v[tn].contains(f)) {
                triangles.insert(st3(f, t, tn));
            }
        }
    }

    std::println("{}", triangles.size());
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/23.txt");
    // first(filename);
    second(filename);
    return 0;
}
