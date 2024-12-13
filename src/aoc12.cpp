#include <array>
#include <deque>

#include "utils.hpp"


using pi = std::pair<int, int>;
// hv
static const std::array<pi, 4> D1 = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};
// dia
static const std::array<pi, 4> D2 = {{{-1, 1}, {1, 1}, {1, -1}, {-1, -1}}};
static const char COUNTED = '.';


auto garden_price(pi start, char plant, std::vector<std::vector<char>>& field)
    -> long {
    std::deque<pi> q;
    std::unordered_set<pi, pair_hash> visited;
    q.push_front(start);
    long area = 0;
    long perimeter = 0;

    auto gh = static_cast<int>(field.size());
    auto gw = static_cast<int>(field[0].size());

    while (!q.empty()) {
        auto [crow, ccol] = q.front();
        q.pop_front();
        auto cur = field[crow][ccol];
        if (cur == COUNTED) {
            continue;
        }
        area += 1;

        short pcell = 4;
        for (auto [rshift, colshift] : D1) {
            auto rs = crow + rshift;
            auto cs = ccol + colshift;
            // out block, not a perim
            if (visited.contains({rs, cs})) {
                pcell -= 1;
                continue;
            }
            // edge, perim
            if (!(rs < gh && rs >= 0 && cs >= 0 && cs < gw)) {
                continue;
            }
            // our block, not seen, go
            if (field[rs][cs] == plant) {
                pcell -= 1;
                q.push_front({rs, cs});
            }
        }
        visited.insert({crow, ccol});
        field[crow][ccol] = COUNTED;
        perimeter += pcell;
    }

    return area * perimeter;
}


auto not_our_garden(int row,
                    int col,
                    const std::unordered_set<pi, pair_hash>& visited,
                    const std::vector<std::vector<char>>& field,
                    char plant) -> bool {
    auto gh = static_cast<int>(field.size());
    auto gw = static_cast<int>(field[0].size());
    return !(row < gh && row >= 0 && col >= 0 && col < gw) ||
           (!visited.contains({row, col}) && field[row][col] != plant);
}


auto garden_price_discounted(pi start,
                             char plant,
                             std::vector<std::vector<char>>& field) -> long {
    std::deque<pi> q;
    std::unordered_set<pi, pair_hash> visited;
    q.push_front(start);
    long area = 0;
    long corners = 0;

    auto gh = static_cast<int>(field.size());
    auto gw = static_cast<int>(field[0].size());
    while (!q.empty()) {
        auto [crow, ccol] = q.front();
        q.pop_front();
        auto cur = field[crow][ccol];
        if (cur == COUNTED) {
            continue;
        }
        area += 1;
        for (auto [rshift, colshift] : D2) {
            auto rs = crow + rshift;
            auto cs = ccol + colshift;
            auto hrow = crow;
            auto hcol = ccol + colshift;
            auto vrow = crow + rshift;
            auto vcol = ccol;

            bool hfree = not_our_garden(hrow, hcol, visited, field, plant);
            bool vfree = not_our_garden(vrow, vcol, visited, field, plant);
            bool diagfree = not_our_garden(rs, cs, visited, field, plant);

            if (hfree && vfree) {
                // outer corner
                corners += 1;
            } else if (!hfree && !vfree && diagfree) {
                // inner corner
                corners += 1;
            }
        }

        // run scan
        for (auto [rshift, colshift] : D1) {
            auto rs = crow + rshift;
            auto cs = ccol + colshift;
            if (!visited.contains({rs, cs}) &&
                (rs < gh && rs >= 0 && cs >= 0 && cs < gw) &&
                field[rs][cs] == plant) {
                q.push_front({rs, cs});
            }
        }

        visited.insert({crow, ccol});
        field[crow][ccol] = COUNTED;
    }

    auto sides = corners;
    return area * sides;
}


auto first(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> g;
    read_lines(file, [&g](std::string line) {
        g.emplace_back(line.begin(), line.end());
    });

    auto gh = static_cast<int>(g.size());
    auto gw = static_cast<int>(g[0].size());
    long price = 0;
    for (auto row = 0; row < gh; row++) {
        for (auto col = 0; col < gw; col++) {
            auto ch = g[row][col];
            if (ch == COUNTED) {
                continue;
            }
            pi coords = {row, col};
            price += garden_price(coords, ch, g);
        }
    }

    std::println("{}", price);
}


auto second(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> g;
    read_lines(file, [&g](std::string line) {
        g.emplace_back(line.begin(), line.end());
    });

    auto gh = static_cast<int>(g.size());
    auto gw = static_cast<int>(g[0].size());
    long price = 0;
    for (auto row = 0; row < gh; row++) {
        for (auto col = 0; col < gw; col++) {
            auto ch = g[row][col];
            if (ch == COUNTED) {
                continue;
            }
            pi coords = {row, col};
            price += garden_price_discounted(coords, ch, g);
        }
    }

    std::println("{}", price);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/12.txt");
    first(filename);
    second(filename);
    return 0;
}
