
#include <queue>
#include "utils.hpp"


// y, x
static const std::array<pi, 4> DIRS = {
    {
        {-1, 0},  // north
        {0, 1},   // east
        {1, 0},   // south
        {0, -1}   // west
    },
};
static const int EAST_I = 1;


auto deer_can_move(std::vector<std::vector<char>>& g, pi pos, pi dir) {
    auto [ny, nx] = (pos + dir);
    return (ny >= 0 && ny < static_cast<int>(g.size()) && nx >= 0 &&
            nx < static_cast<int>(g[0].size()) && g[ny][nx] != '#' &&
            g[ny][nx] != 'b');
}


// failed to add backtracking here, subtle error around rotation...
auto deer(std::vector<std::vector<char>>& grid, pi start, pi end) -> long {
    // cost, pos, diri
    std::priority_queue<std::tuple<long, pi, int>,
                        std::vector<std::tuple<long, pi, int>>,
                        std::greater<>>
        pq;

    // y, x, diri = > cost
    std::unordered_map<std::tuple<int, int, int>, long, tuple_hash> visited;
    pq.push({0, start, EAST_I});

    while (!pq.empty()) {
        auto [cost, pos, dir_i] = pq.top();
        pq.pop();
        if (pos == end) {
            return cost;
        }
        auto [py, px] = pos;
        auto t = std::make_tuple(py, px, dir_i);
        if (visited.contains(t) && visited[t] <= cost) {
            continue;
        }

        auto dir = DIRS[dir_i];
        visited[t] = cost;

        if (deer_can_move(grid, pos, dir)) {
            pq.push({cost + 1, pos + dir, dir_i});
        }

        pq.push({cost + 1000, pos, wrap(dir_i + 1, 4)});
        pq.push({cost + 1000, pos, wrap(dir_i - 1, 4)});
    }

    return -1;
}


// y, x, diri
using tri = std::tuple<int, int, int>;

// couldn't find some subtle bug in normal deer impl when collecting paths..
auto backtracking_deer(std::vector<std::vector<char>>& grid, pi start, pi end)
    -> std::pair<tri,
                 std::unordered_map<tri,
                                    std::unordered_set<tri, tuple_hash>,
                                    tuple_hash>> {
    // cost, pos, diri
    std::priority_queue<std::tuple<long, pi, int>,
                        std::vector<std::tuple<long, pi, int>>,
                        std::greater<>>
        pq;

    // y, x, dir
    std::unordered_map<tri, std::unordered_set<tri, tuple_hash>, tuple_hash>
        prevs;

    // y, x, diri => cost
    std::unordered_map<tri, long, tuple_hash> costs;
    costs[{start.first, start.second, EAST_I}] = 0;
    pq.push({0, start, EAST_I});

    while (!pq.empty()) {
        auto [cost, pos, dir_i] = pq.top();
        pq.pop();
        auto [py, px] = pos;
        auto vk = std::make_tuple(py, px, dir_i);
        if (cost > costs[vk]) {
            continue;
        }

        std::array<std::pair<long, int>, 3> directions = {{
            {cost + 1, dir_i},
            {cost + 1000, wrap(dir_i + 1, 4)},
            {cost + 1000, wrap(dir_i - 1, 4)},
        }};
        for (auto [new_cost, ndir_i] : directions) {
            auto dir = DIRS[ndir_i];
            if (deer_can_move(grid, pos, dir)) {
                auto [ny, nx] = pos + dir;
                auto t = std::make_tuple(ny, nx, ndir_i);
                if (!costs.contains(t) || costs[t] > new_cost) {
                    prevs[t] = {vk};
                    costs[t] = new_cost;
                    pq.push({new_cost, {ny, nx}, ndir_i});
                } else if (costs.contains(t) && costs[t] == new_cost) {
                    prevs[t].insert(vk);
                }
            }
        }
    }

    auto min_cost = LONG_MAX;
    tri k = {-1, -1, -1};
    for (auto i = 0; i < 4; i++) {
        tri tk = {end.first, end.second, i};
        if (costs.contains(tk) && costs[tk] < min_cost) {
            min_cost = costs[tk];
            k = tk;
        }
    }

    return {k, prevs};
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

    auto [dir_end, backtrack] = backtracking_deer(grid, start, end);
    if (!backtrack.contains(dir_end)) {
        std::println("bug");
        return;
    }
    std::unordered_set<tri, tuple_hash> moves;
    moves.insert({start.first, start.second, EAST_I});
    moves.insert(dir_end);
    std::vector<tri> to_check;
    to_check.push_back(dir_end);
    while (!to_check.empty()) {
        auto tile = to_check.back();
        to_check.pop_back();
        for (auto another : backtrack[tile]) {
            if (!moves.contains(another)) {
                to_check.push_back(another);
                moves.insert(another);
            }
        }
    }
    std::unordered_set<pi, pair_hash> seats;
    for (auto [y, x, _] : moves) {
        seats.insert({y, x});
    }
    std::println("{}", seats.size());
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

    std::println("{}", deer(grid, start, end));
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/16.txt");
    first(filename);
    second(filename);
    return 0;
}
