
#include "utils.hpp"

using pi = std::pair<int, int>;

static constexpr pi UP{-1, 0};
static constexpr pi DOWN{1, 0};
static constexpr pi LEFT{0, -1};
static constexpr pi RIGHT{0, 1};

std::unordered_map<pi, pi, pair_hash> TURNS{{UP, RIGHT},
                                            {RIGHT, DOWN},
                                            {DOWN, LEFT},
                                            {LEFT, UP}};

std::unordered_map<char, pi> DIRS{{'<', LEFT},
                                  {'>', RIGHT},
                                  {'^', UP},
                                  {'v', DOWN}};

auto first(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> grid;
    int gx, gy;
    pi gdir;
    int r = 0;
    bool found = false;

    read_lines(file, [&r, &grid, &gdir, &gx, &gy, &found](std::string line) {
        grid.emplace_back(line.begin(), line.end());
        if (!found) {
            auto p = line.find_first_of("<^v>");
            if (p != std::string::npos) {
                auto g = line[p];
                gdir = DIRS[g];
                gx = static_cast<int>(p);
                gy = r;
                found = true;
            }
        }
        r++;
    });

    int gwidth = grid[0].size();
    int gsize = grid.size();
    std::unordered_set<pi, pair_hash> visited;
    visited.insert({gx, gy});
    int tx, ty;
    while (true) {
        ty = gy + gdir.first;
        tx = gx + gdir.second;
        if (ty < 0 || ty >= gsize || tx < 0 || tx >= gwidth) {
            break;
        }

        auto pos = grid[ty][tx];
        if (pos == '#') {
            gdir = TURNS[gdir];
        } else {
            gx = tx;
            gy = ty;
            visited.insert({gx, gy});
        }
    }
    std::println("{}", visited.size());
}

auto second(std::string s) -> void {
    std::ifstream file(s);
    std::vector<std::vector<char>> grid;
    int gx, gy;
    pi gdir;
    int r = 0;
    bool found = false;

    read_lines(file, [&r, &grid, &gdir, &gx, &gy, &found](std::string line) {
        grid.emplace_back(line.begin(), line.end());
        if (!found) {
            auto p = line.find_first_of("<^v>");
            if (p != std::string::npos) {
                auto g = line[p];
                gdir = DIRS[g];
                gx = static_cast<int>(p);
                gy = r;
                found = true;
            }
        }
        r++;
    });

    int gwidth = grid[0].size();
    int gsize = grid.size();
    std::unordered_set<pi, pair_hash> visited;
    int ox = gx, oy = gy;
    int tx, ty;
    while (true) {
        ty = gy + gdir.first;
        tx = gx + gdir.second;
        if (ty < 0 || ty >= gsize || tx < 0 || tx >= gwidth) {
            break;
        }

        auto pos = grid[ty][tx];
        if (pos == '#') {
            gdir = TURNS[gdir];
        } else {
            gx = tx;
            gy = ty;
            visited.insert({gy, gx});
        }
    }
    // find cycles
    visited.erase({ox, oy});  // no orig
    int cycles = 0;
    for (auto obst : visited) {
        gdir = UP;
        auto newg = deep_copy(grid);
        gx = ox;
        gy = oy;
        newg[obst.first][obst.second] = '#';
        std::unordered_set<std::tuple<int, int, int, int>, tuple_hash> vdir;
        while (true) {
            ty = gy + gdir.first;
            tx = gx + gdir.second;
            if (ty < 0 || ty >= gsize || tx < 0 || tx >= gwidth) {
                break;
            }

            auto pos = newg[ty][tx];
            if (pos == '#') {
                gdir = TURNS[gdir];
            } else {
                gx = tx;
                gy = ty;
                auto [_, ok] = vdir.insert({gy, gx, gdir.first, gdir.second});
                if (!ok) {
                    cycles += 1;
                    break;
                }
            }
        }
    }
    std::println("{}", cycles);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/6.txt");
    first(filename);
    second(filename);
    return 0;
}
