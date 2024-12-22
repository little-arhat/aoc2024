#include <deque>
#include "utils.hpp"

template <typename T>
auto ins(T& v, int d, char fill) {
    if (d != 0) {
        v.append(std::abs(d), fill);
    }
}


// clang-format off
//     +---+---+
//     | ^ | A |
// +---+---+---+
// | < | v | > |
// +---+---+---+
static const std::unordered_map<char, std::pair<int, int>> DIRPAD = {
                   {'^', {0, 1}}, {'A', {0, 2}},
    {'<', {1, 0}}, {'v', {1, 1}}, {'>', {1, 2}},
};
static const std::pair DIR_GAP = {0, 0};


// +---+---+---+
// | 7 | 8 | 9 |
// +---+---+---+
// | 4 | 5 | 6 |
// +---+---+---+
// | 1 | 2 | 3 |
// +---+---+---+
//     | 0 | A |
//     +---+---+
static const std::unordered_map<char, std::pair<int, int>> NUMPAD = {
    {'7', {0, 0}}, {'8', {0, 1}}, {'9', {0, 2}},
    {'4', {1, 0}}, {'5', {1, 1}}, {'6', {1, 2}},
    {'1', {2, 0}}, {'2', {2, 1}}, {'3', {2, 2}},
    {'0', {3, 1}}, {'A', {3, 2}}};
static const std::pair NUM_GAP = {3, 0};
// clang-format on


static const std::string NO_GAP_PLEASE = "*";


auto pad_step(const char from,
              const char to,
              const std::unordered_map<char, std::pair<int, int>>& pad_map,
              const std::pair<int, int> gap) -> std::string {
    auto [st_y, st_x] = pad_map.at(from);
    auto [e_y, e_x] = pad_map.at(to);
    auto dy = e_y - st_y;
    auto dx = e_x - st_x;
    std::string r;
    r.reserve(std::abs(dy) + std::abs(dx) + 1);

    if (std::make_pair(st_x, e_y) != gap && dy > 0) {
        ins(r, dy, 'v');
    }
    if (dx > 0) {
        ins(r, dx, '>');
    }
    if (std::make_pair(st_x, e_y) == gap && dy > 0) {
        ins(r, dy, 'v');
    }
    if (std::make_pair(e_x, st_y) != gap && dx < 0) {
        ins(r, dx, '<');
    }
    if (dy < 0) {
        ins(r, dy, '^');
    }
    if (std::make_pair(e_x, st_y) == gap && dx < 0) {
        ins(r, dx, '<');
    }
    r.push_back('A');

    return r;
}


auto pad(const std::string& command,
         const std::unordered_map<char, std::pair<int, int>>& pad_map,
         const std::pair<int, int> gap) -> std::string {
    std::vector<char> r;
    char start = 'A';
    for (size_t i = 0; i < command.size(); i++) {
        char end = command[i];
        auto rr = pad_step(start, end, pad_map, gap);
        r.insert(r.end(), rr.begin(), rr.end());
        start = end;
    }

    return std::string(r.begin(), r.end());
}


auto dir_pad(const std::string& command) -> std::string {
    return pad(command, DIRPAD, DIR_GAP);
}


auto num_pad(const std::string& code) -> std::string {
    return pad(code, NUMPAD, NUM_GAP);
}


auto pad_step_dumb(const char from,
                   const char to,
                   const std::unordered_map<char, std::pair<int, int>>& pad_map,
                   const std::pair<int, int> gap)
    -> std::pair<std::string, std::string> {
    auto [st_y, st_x] = pad_map.at(from);
    auto [e_y, e_x] = pad_map.at(to);
    auto dy = e_y - st_y;
    auto dx = e_x - st_x;
    std::string r1;
    std::string r2;
    r1.reserve(std::abs(dy) + std::abs(dx) + 1);
    r2.reserve(std::abs(dy) + std::abs(dx) + 1);

    auto ud = dy > 0 ? 'v' : '^';
    auto lr = dx > 0 ? '>' : '<';

    ins(r1, dy, ud);
    ins(r1, dx, lr);
    r1.push_back('A');

    ins(r2, dx, lr);
    ins(r2, dy, ud);
    r2.push_back('A');

    return {
        std::make_pair(e_y, st_x) != gap ? r1 : NO_GAP_PLEASE,
        std::make_pair(st_y, e_x) != gap ? r2 : NO_GAP_PLEASE,
    };
}


auto dir_pad_rec(
    const std::string& command,
    // code, level => length
    std::unordered_map<std::pair<std::string, int>, unsigned long, pair_hash>&
        cache,
    int level,
    int max_levels) -> unsigned long {
    if (level == max_levels) {
        return command.size();
    }
    if (cache.contains({command, level})) {
        return cache[{command, level}];
    }

    char start = 'A';
    unsigned long command_sum = 0;
    for (size_t i = 0; i < command.size(); i++) {
        char end = command[i];
        auto [r1, r2] = level == 0 ? pad_step_dumb(start, end, NUMPAD, NUM_GAP)
                                   : pad_step_dumb(start, end, DIRPAD, DIR_GAP);
        auto r1_len = r1 != NO_GAP_PLEASE
                          ? dir_pad_rec(r1, cache, level + 1, max_levels)
                          : ULONG_MAX;
        auto r2_len = r2 != NO_GAP_PLEASE
                          ? dir_pad_rec(r2, cache, level + 1, max_levels)
                          : ULONG_MAX;
        unsigned long se_length = std::min(r1_len, r2_len);
        command_sum += se_length;
        start = end;
    }
    cache[{command, level}] = command_sum;
    return command_sum;
}


auto second(std::string s) -> void {
    unsigned long long complexities = 0;

    read_lines(s, [&complexities](std::string code) {
        if (code.empty()) {
            return;
        }

        std::
            unordered_map<std::pair<std::string, int>, unsigned long, pair_hash>
                cache;

        auto len = dir_pad_rec(code, cache, 0, 26);
        auto num = std::stoi(code.substr(0, code.size() - 1));
        complexities += (num * len);
    });

    std::println("{}", complexities);
}


auto first(std::string s) -> void {
    long complexities = 0;
    read_lines(s, [&complexities](std::string code) {
        if (code.empty()) {
            return;
        }
        auto num_robot = num_pad(code);
        auto first_dir_robot = dir_pad(num_robot);
        auto me = dir_pad(first_dir_robot);
        auto num = std::stoi(code.substr(0, code.size() - 1));
        complexities += (num * me.size());
    });

    std::println("{}", complexities);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/21.txt");
    first(filename);
    second(filename);
    return 0;
}
