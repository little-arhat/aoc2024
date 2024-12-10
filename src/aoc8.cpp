
#include <cmath>
#include "utils.hpp"

using pi = std::pair<int, int>;


auto find_harmonics(pi older,
                    pi newer,
                    int width,
                    int height) -> std::vector<pi> {
    auto vdiff = std::abs(older.first - newer.first);
    auto hdiff = std::abs(older.second - newer.second);
    auto bl_tr = older.second > newer.second;

    auto newf = [&bl_tr, &vdiff, &hdiff](auto np) {
        if (bl_tr) {
            return std::make_pair(np.first + vdiff, np.second - hdiff);
        } else {
            return std::make_pair(np.first + vdiff, np.second + hdiff);
        }
    };

    auto oldf = [&bl_tr, &vdiff, &hdiff](auto op) {
        if (bl_tr) {
            return std::make_pair(op.first - vdiff, op.second + hdiff);
        } else {
            return std::make_pair(op.first - vdiff, op.second - hdiff);
        }
    };
    std::vector<pi> r;
    auto p = newf(newer);
    while (p.first >= 0 && p.second >= 0 && p.first < height &&
           p.second < width) {
        r.push_back(p);
        p = newf(p);
    }
    p = oldf(older);
    while (p.first >= 0 && p.second >= 0 && p.first < height &&
           p.second < width) {
        r.push_back(p);
        p = oldf(p);
    }
    r.push_back(older);
    r.push_back(newer);
    return r;
}


auto find_antinodes(pi older, pi newer) -> std::pair<pi, pi> {
    auto vdiff = std::abs(older.first - newer.first);
    auto hdiff = std::abs(older.second - newer.second);
    // y coord is always same or greater
    if (older.second > newer.second) {
        // bottom left -> top right
        return {{older.first - vdiff, older.second + hdiff},
                {newer.first + vdiff, newer.second - hdiff}};

    } else {
        // top left -> bottom right
        return {{newer.first + vdiff, newer.second + hdiff},
                {older.first - vdiff, older.second - hdiff}};
    }
}


auto first(std::string s) -> void {
    std::ifstream file(s);

    std::unordered_set<pi, pair_hash> nodes;
    std::unordered_map<char, std::unordered_set<pi, pair_hash>> antennas;

    int w = -1;
    int h = 0;

    read_lines(file, [&nodes, &antennas, &h, &w](std::string line) {
        int c = 0;
        for (auto ch : line) {
            pi newer = {h, c};
            if (ch != '.') {
                for (auto older : antennas[ch]) {
                    auto [a, b] = find_antinodes(older, newer);
                    nodes.insert(a);
                    nodes.insert(b);
                }
                antennas[ch].insert(newer);
            }
            c++;
        }

        h++;
        w = line.size();
    });

    auto results = std::ranges::to<std::unordered_set<pi, pair_hash>>(
        nodes | std::views::filter([&h, &w](const auto& p) {
            return p.first >= 0 && p.second >= 0 && p.first < h && p.second < w;
        }));
    std::println("{}", results.size());
}


auto second(std::string s) -> void {
    std::ifstream file(s);

    std::vector<std::vector<char>> g;

    read_lines(file, [&g](std::string line) {
        g.emplace_back(line.begin(), line.end());
    });
    int w = g[0].size();
    int h = g.size();

    std::unordered_set<pi, pair_hash> nodes;
    std::unordered_map<char, std::unordered_set<pi, pair_hash>> antennas;
    int y = 0;
    for (auto row : g) {
        int x = 0;
        for (auto ch : row) {
            pi newer = {y, x};
            if (ch != '.') {
                for (auto older : antennas[ch]) {
                    auto a = find_harmonics(older, newer, w, h);
                    nodes.insert(a.begin(), a.end());
                }
                antennas[ch].insert(newer);
            }

            x++;
        }
        y++;
    }

    std::println("{}", nodes.size());
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/8.txt");
    first(filename);
    second(filename);
    return 0;
}
