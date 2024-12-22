
#include <queue>
#include "utils.hpp"

// y, x
static const std::array<pi, 4> DIRS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}},
};


auto can_move(std::vector<std::vector<char>>& g, pi npos) {
    auto [ny, nx] = npos;
    return (ny >= 0 && ny < static_cast<int>(g.size()) && nx >= 0 &&
            nx < static_cast<int>(g[0].size()) && g[ny][nx] != '#');
}


auto race(std::vector<std::vector<char>>& grid, pi start, pi end)
    -> std::unordered_map<pi, pi, pair_hash> {
    // estimate, cost, pos
    std::priority_queue<std::tuple<long, long, pi>,
                        std::vector<std::tuple<long, long, pi>>,
                        std::greater<>>
        pq;

    // paths
    std::unordered_map<pi, pi, pair_hash> prevs;

    // y, x = > cost
    std::unordered_map<pi, long, pair_hash> visited;
    pq.push({manhattan(start, end), 0, start});

    while (!pq.empty()) {
        auto [estimate, cost, pos] = pq.top();
        pq.pop();
        if (pos == end) {
            return prevs;
        }
        for (auto dir : DIRS) {
            if (can_move(grid, pos + dir)) {
                auto new_cost = cost + 1;
                auto npos = pos + dir;
                if (!visited.contains(npos) || visited[npos] > new_cost) {
                    visited[npos] = new_cost;
                    prevs[npos] = pos;
                    pq.push({manhattan(npos, end), new_cost, npos});
                }
            }
        }
    }

    return prevs;
}


void print_grid(const std::vector<std::vector<char>>& grid) {
    auto rr = 0;
    std::print(" ");
    for (size_t cc = 0; cc < grid[0].size(); cc++) {
        std::print("{}", cc % 10);
    }
    std::println("");

    for (const auto& row : grid) {
        std::print("{}", rr % 10);
        rr++;
        for (const auto& cell : row) {
            if (cell == '@') {
                std::print("\033[38;5;51m@\033[0m");
            } else if (cell == 'W') {
                std::print("\033[1m#\033[0m");
            } else if (cell == 'O' || cell == '[' || cell == ']') {
                std::print("\033[38;5;196m{}\033[0m", cell);
            } else {
                std::print("{}", cell);
            }
        }
        std::println("");
    }
    std::cout.flush();
}


auto second(std::string s) -> void {
    std::vector<std::vector<char>> grid;
    pi start = {0, 0};
    pi end = {0, 0};

    read_lines(s, [&grid, &start, &end](std::string line) {
        grid.emplace_back(line.begin(), line.end());
        auto p = line.find('E');
        if (p != std::string::npos) {
            end = {grid.size() - 1, p};
        }
        p = line.find('S');
        if (p != std::string::npos) {
            start = {grid.size() - 1, p};
        }
    });
    auto min_cheat = grid.size() > 100 ? 100 : 50;
    auto cheat_distance = 20;

    auto prevs = race(grid, start, end);
    std::deque<std::pair<pi, long>> path;
    long distance = 0;
    auto cur = end;
    do {
        path.push_front({cur, distance});
        cur = prevs[cur];
        distance += 1;
    } while (cur != start);
    path.push_front({start, distance});

    long cheats = 0;
    for (auto outer = path.begin(); outer != path.end(); ++outer) {
        auto [step, distance] = *outer;
        for (auto inner = std::next(outer); inner != path.end(); ++inner) {
            auto [other_step, other_distance] = *inner;
            auto jump = manhattan(step, other_step);
            auto save = distance - (jump + other_distance);
            if (cheat_distance >= jump && save >= min_cheat) {
                cheats += 1;
            }
        }
    }


    std::println("{}", cheats);
}


auto first(std::string s) -> void {
    std::vector<std::vector<char>> grid;
    pi start = {0, 0};
    pi end = {0, 0};

    read_lines(s, [&grid, &start, &end](std::string line) {
        grid.emplace_back(line.begin(), line.end());
        auto p = line.find('E');
        if (p != std::string::npos) {
            end = {grid.size() - 1, p};
        }
        p = line.find('S');
        if (p != std::string::npos) {
            start = {grid.size() - 1, p};
        }
    });
    long min_cheat = grid.size() > 100 ? 100 : 2;

    auto prevs = race(grid, start, end);
    std::unordered_map<pi, long, pair_hash> distances;
    std::deque<pi> path;
    long distance = 0;
    auto cur = end;
    do {
        path.push_front(cur);
        distances[cur] = distance;
        cur = prevs[cur];
        distance += 1;
    } while (cur != start);
    path.push_front(start);
    distances[start] = distance;

    long cheats = 0;
    for (auto step : path) {
        distance = distances[step];
        for (auto dir : DIRS) {
            auto cheating_dir = dir * 2;
            auto no_time_travel = step + dir;
            auto jump = step + cheating_dir;
            if (!can_move(grid, no_time_travel) && can_move(grid, jump) &&
                distances.contains(jump)) {
                auto with_cheat = distances[jump] + 2;
                if (distance - with_cheat >= min_cheat) {
                    cheats += 1;
                }
            }
        }
    }

    std::println("{}", cheats);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/20.txt");
    first(filename);
    second(filename);
    return 0;
}
