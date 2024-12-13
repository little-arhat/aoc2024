#include <array>
#include <deque>

#include "utils.hpp"


using pi = std::pair<int, int>;
static const std::array<pi, 4> D1 = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};


auto first(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<int>> g;
    std::deque<pi> next;
    read_lines(file, [&g, &next](std::string line) {
        g.push_back(std::vector<int>());
        auto& row = g[g.size() - 1];
        for (auto c : line) {
            auto i = c == '.' ? -1 : digit(c);
            row.push_back(i);
            if (i == 0) {
                next.push_back({g.size() - 1, row.size() - 1});
            }
        }
    });

    long score = 0;
    std::unordered_set<pi, pair_hash> nines;
    auto gh = static_cast<int>(g.size());
    auto gw = static_cast<int>(g[0].size());
    while (!next.empty()) {
        auto [crow, ccol] = next.front();
        next.pop_front();

        auto val = g[crow][ccol];
        if (val == 9) {
            nines.insert({crow, ccol});
            continue;
        } else if (val == 0) {
            score += nines.size();
            nines.clear();
        }

        for (auto [rshift, colshift] : D1) {
            auto rs = crow + rshift;
            auto cs = ccol + colshift;
            if (rs < gh && rs >= 0 && cs >= 0 && cs < gw) {
                auto nv = g[rs][cs];
                if (nv - val == 1) {
                    next.push_front({rs, cs});
                }
            }
        }
    }
    score += nines.size();

    std::println("{}", score);
}

auto second(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<int>> g;
    std::deque<pi> next;
    read_lines(file, [&g, &next](std::string line) {
        g.push_back(std::vector<int>());
        auto& row = g[g.size() - 1];
        for (auto c : line) {
            auto i = c == '.' ? -1 : digit(c);
            row.push_back(i);
            if (i == 0) {
                next.push_back({g.size() - 1, row.size() - 1});
            }
        }
    });

    long score = 0;
    long rating = 0;
    auto gh = static_cast<int>(g.size());
    auto gw = static_cast<int>(g[0].size());
    while (!next.empty()) {
        auto [crow, ccol] = next.front();
        next.pop_front();

        auto val = g[crow][ccol];
        if (val == 9) {
            rating++;
            continue;
        } else if (val == 0) {
            score += rating;
            rating = 0;
        }

        for (auto [rshift, colshift] : D1) {
            auto rs = crow + rshift;
            auto cs = ccol + colshift;
            if (rs < gh && rs >= 0 && cs >= 0 && cs < gw) {
                auto nv = g[rs][cs];
                if (nv - val == 1) {
                    next.push_front({rs, cs});
                }
            }
        }
    }
    score += rating;

    std::println("{}", score);
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/10.txt");
    first(filename);
    second(filename);
    return 0;
}
