
#include <queue>
#include <regex>
#include <tuple>
#include "utils.hpp"

using pi = std::pair<int, int>;
// unwrapped (cost, moves_a, moves_b, x, y)
using el = std::tuple<long, int, int, int, int>;

struct Input {
    pi a, b, prize;

    auto to_string() const -> const std::string {
        return std::format("{{A=({}, {}); B=({}, {}); p=({}, {})}}",
                           a.first,
                           a.second,
                           b.first,
                           b.second,
                           prize.first,
                           prize.second);
    }
};

template <>
struct std::formatter<Input> : std::formatter<std::string> {
    auto format(Input i, format_context& ctx) const {
        return formatter<string>::format(i.to_string(), ctx);
    }
};


auto guldens(Input i) -> std::optional<long> {
    std::priority_queue<el, std::vector<el>, std::greater<>> pq;
    std::unordered_set<std::tuple<int, int, int, int>, tuple_hash> visited;
    pq.push({0, 0, 0, 0, 0});
    auto [px, py] = i.prize;
    auto [ax, ay] = i.a;
    auto [bx, by] = i.b;

    while (!pq.empty()) {
        auto [cost, moves_a, moves_b, x, y] = pq.top();
        pq.pop();
        if (x == px && y == py) {
            return cost;
        }
        if (visited.contains({x, y, moves_a, moves_b})) {
            continue;
        }
        visited.insert({x, y, moves_a, moves_b});
        if (moves_a < 100) {
            pq.push({cost + 3, moves_a + 1, moves_b, x + ax, y + ay});
        }
        if (moves_b < 100) {
            pq.push({
                cost + 1,
                moves_a,
                moves_b + 1,
                x + bx,
                y + by,
            });
        }
    }

    return {};
}


auto parse_input(const std::string& text) -> std::vector<Input> {
    std::vector<Input> result;
    std::regex pattern(
        R"(Button A: X\+(\d+), Y\+(\d+)\s+Button B: X\+(\d+), Y\+(\d+)\s+Prize: X=(\d+), Y=(\d+))");
    std::smatch match;

    auto start = text.cbegin();
    while (std::regex_search(start, text.cend(), match, pattern)) {
        result.push_back({{std::stoi(match[1]), std::stoi(match[2])},
                          {std::stoi(match[3]), std::stoi(match[4])},
                          {std::stoi(match[5]), std::stoi(match[6])}});
        start = match.suffix().first;
    }
    return result;
}


auto first(std::string s) -> void {
    std::string inp = slurp(s);
    std::vector<Input> i = parse_input(inp);

    long tokens = 0;
    for (auto ai : i) {
        auto a = guldens(ai);
        if (a) {
            tokens += a.value();
        }
    }

    std::println("{}", tokens);
}


auto second(std::string s) -> void {
    std::string inp = slurp(s);
    std::println("{}", inp.size());
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/13.txt");
    first(filename);
    // second(filename);
    return 0;
}
