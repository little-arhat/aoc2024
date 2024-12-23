
#include <algorithm>
#include "utils.hpp"

using str = std::string;
using strset = std::unordered_set<str>;
using graph = std::unordered_map<str, std::unordered_set<str>>;

struct array_cmp {
    bool operator()(const std::array<str, 3>& lhs,
                    const std::array<str, 3>& rhs) const {
        return lhs < rhs;
    }
};


template <typename T>
auto st3(T a, T b, T c) -> std::array<T, 3> {
    std::array<T, 3> arr = {a, b, c};
    std::sort(arr.begin(), arr.end());
    return arr;
}

auto chief(const str& s) -> bool {
    return s[0] == 't';
}


template <typename K, typename V>
auto keys(const std::unordered_map<K, V>& map) -> std::unordered_set<K> {
    return std::unordered_set<K>(std::ranges::views::keys(map).begin(),
                                 std::ranges::views::keys(map).end());
}


auto intersection(const strset& a, const strset& b) -> strset {
    strset r;
    for (auto x : a) {
        if (b.contains(x)) {
            r.insert(x);
        }
    }
    return r;
}


auto bron_kerbosch(strset R,
                   strset P,
                   strset X,
                   graph& graph,
                   std::vector<strset>& result) -> void {
    if (P.empty() && X.empty()) {
        result.push_back(R);
        return;
    }

    for (auto it = P.begin(); it != P.end();) {
        auto node = *it;
        auto connections = graph[node];

        bron_kerbosch(intersection(R, {node}),
                      intersection(P, connections),
                      intersection(X, connections),
                      graph,
                      result);

        it = P.erase(it);
        X.insert(node);
    }
}


auto largest_clique(graph& g) -> strset {
    std::vector<strset> cliques;
    bron_kerbosch({}, keys(g), {}, g, cliques);
    auto a = std::max_element(
        cliques.begin(),
        cliques.end(),
        [](strset& a, strset& b) { return a.size() < b.size(); });
    return *a;
}


auto second(str s) {
    graph g;
    read_lines(s, [&g](str line) {
        auto pos = line.find('-');
        auto f = line.substr(0, pos);
        auto t = line.substr(pos + 1);
        g[f].insert(t);
        g[t].insert(f);
    });

    auto largest = largest_clique(g);
    std::set<str> password{largest.begin(), largest.end()};
    std::println("{}", join_str(password, ','));
}


auto first(str s) {
    graph v;
    std::vector<std::pair<str, str>> e;
    read_lines(s, [&v, &e](str line) {
        auto pos = line.find('-');
        auto f = line.substr(0, pos);
        auto t = line.substr(pos + 1);
        e.push_back({f, t});
        v[f].insert(t);
        v[t].insert(f);
    });

    std::unordered_set<std::array<str, 3>, array_hash> triangles;
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
    str filename = aoc(argc, argv, "../inputs/23.txt");
    first(filename);
    second(filename);
    return 0;
}
