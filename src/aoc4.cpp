
#include <array>
#include <ranges>

#include "utils.hpp"

static const std::array<std::string, 2> W1 = {"XMAS", "SAMX"};
// ROW, COL
static const std::array<std::pair<int, int>, 4> D1 = {
    {{1, 0}, {0, 1}, {1, 1}, {-1, 1}}};


auto first(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> g;
    read_lines(file, [&g](std::string line) {
        g.emplace_back(line.begin(), line.end());
    });

    int found = 0;
    auto gsize = static_cast<int>(g.size());
    for (auto r : std::views::iota(0, gsize)) {
        auto gwidth = static_cast<int>(g[r].size());
        for (auto c : std::views::iota(0, gwidth)) {
            for (auto [rshift, colshift] : D1) {
                std::string word;

                for (auto i : std::views::iota(0, 4)) {
                    auto rs = r + rshift * i;
                    auto cs = c + colshift * i;
                    if (rs < gsize && rs >= 0 && cs >= 0 && cs < gwidth) {
                        word += g[rs][cs];
                    } else {
                        break;
                    }
                }
                for (auto w : W1) {
                    if (w == word) {
                        found += 1;
                        break;
                    }
                }
            }
        }
    }
    std::println("{}", found);
}


auto second(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> g;
    read_lines(file, [&g](std::string line) {
        g.emplace_back(line.begin(), line.end());
    });

    int found = 0;
    auto gsize = static_cast<int>(g.size());
    for (auto r : std::views::iota(1, gsize - 1)) {
        auto gwidth = static_cast<int>(g[r].size());
        for (auto c : std::views::iota(1, gwidth - 1)) {
            std::string lu_rb{g[r - 1][c - 1], g[r][c], g[r + 1][c + 1]};
            std::string lb_ru{g[r + 1][c - 1], g[r][c], g[r - 1][c + 1]};
            if ((lu_rb == "MAS" || lu_rb == "SAM") &&
                (lb_ru == "MAS" || lb_ru == "SAM")) {
                found += 1;
            }
        }
    }

    std::println("{}", found);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/4.txt");
    first(filename);
    second(filename);
    return 0;
}
