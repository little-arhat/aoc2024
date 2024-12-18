
#include <queue>
#include "utils.hpp"

// y, x
static const std::array<pi, 4> DIRS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}},
};


auto can_move(std::vector<std::vector<char>>& g, pi pos, pi dir) {
    auto [ny, nx] = (pos + dir);
    return (ny >= 0 && ny < static_cast<int>(g.size()) && nx >= 0 &&
            nx < static_cast<int>(g[0].size()) && g[ny][nx] != '#');
}


auto dutch_walk_no_star(std::vector<std::vector<char>>& grid,
                        pi start,
                        pi end) -> long {
    // cost, pos
    std::priority_queue<std::tuple<long, pi>,
                        std::vector<std::tuple<long, pi>>,
                        std::greater<>>
        pq;

    // y, x = > cost
    std::unordered_map<pi, long, pair_hash> visited;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [cost, pos] = pq.top();
        pq.pop();
        if (pos == end) {
            return cost;
        }
        if (visited.contains(pos) && cost > visited[pos]) {
            continue;
        }
        for (auto dir : DIRS) {
            auto new_cost = cost + 1;
            if (can_move(grid, pos, dir)) {
                auto npos = pos + dir;
                if (!visited.contains(npos) || visited[npos] > new_cost) {
                    visited[npos] = new_cost;
                    pq.push({new_cost, npos});
                }
            }
        }
    }

    return -1;
}


auto reset(std::vector<std::vector<char>>& g) -> void {
    for (size_t r = 0; r < (g.size()); r++) {
        for (size_t c = 0; c < (g[0].size()); c++) {
            g[r][c] = '.';
        }
    }
}


auto second(std::string s) -> void {
    std::vector<pi> obstacles;
    int maxs = INT_MIN;

    read_lines(s, [&obstacles, &maxs](std::string line) {
        auto [x, y] = to_pair<int>(line, ',');
        obstacles.push_back({y, x});
        maxs = std::max(maxs, y);
        maxs = std::max(maxs, x);
    });
    pi start = {0, 0};
    pi end = {maxs, maxs};
    std::vector<std::vector<char>> grid(maxs + 1,
                                        std::vector<char>(maxs + 1, '.'));

    size_t R = obstacles.size();
    size_t L = obstacles.size() > 100 ? 1024 : 12;
    auto answer = -1;
    while (L <= R) {
        reset(grid);
        int mid = (R + L) / 2;
        for (size_t i = 0; i < std::min<size_t>(mid, obstacles.size()); ++i) {
            auto [row, col] = obstacles[i];
            grid[row][col] = '#';
        }
        auto step_ans = dutch_walk_no_star(grid, start, end);
        if (step_ans == -1) {
            answer = mid;
            R = mid - 1;
        } else {
            L = mid + 1;
        }
    }

    auto [y, x] = obstacles[answer - 1];
    std::println("{}: {},{}", answer, x, y);
}

auto first(std::string s) -> void {
    std::vector<pi> obstacles;
    int maxs = INT_MIN;

    // todo: add read tuples?
    read_lines(s, [&obstacles, &maxs](std::string line) {
        auto [x, y] = to_pair<int>(line, ',');
        obstacles.push_back({y, x});
        maxs = std::max(maxs, y);
        maxs = std::max(maxs, x);
    });
    pi start = {0, 0};
    pi end = {maxs, maxs};
    std::vector<std::vector<char>> grid(maxs + 1,
                                        std::vector<char>(maxs + 1, '.'));
    size_t N = obstacles.size() > 100 ? 1024 : 12;  // fng
    for (size_t i = 0; i < std::min<size_t>(N, obstacles.size()); ++i) {
        auto [row, col] = obstacles[i];
        grid[row][col] = '#';
    }

    std::println("{}", dutch_walk_no_star(grid, start, end));
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/18.txt");
    // first(filename);
    second(filename);
    return 0;
}
