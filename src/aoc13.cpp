
#include <queue>
#include <regex>
#include <tuple>
#include "utils.hpp"

using pi = std::pair<long, long>;
// unwrapped (cost, moves_a, moves_b, x, y)
using el = std::tuple<long, int, int, long, long>;
// unwrapped (cost, x, y)
using el2 = std::tuple<long, long, long>;

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
    // x, y, movea, moveb
    std::unordered_set<std::tuple<long, long, long, long>, tuple_hash> visited;
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


auto parse_input(const std::string& text,
                 long prize_correction) -> std::vector<Input> {
    std::vector<Input> result;
    std::regex pattern(
        R"(Button A: X\+(\d+), Y\+(\d+)\s+Button B: X\+(\d+), Y\+(\d+)\s+Prize: X=(\d+), Y=(\d+))");
    std::smatch match;

    auto start = text.cbegin();
    while (std::regex_search(start, text.cend(), match, pattern)) {
        result.push_back({{std::stoi(match[1]), std::stoi(match[2])},
                          {std::stoi(match[3]), std::stoi(match[4])},
                          {prize_correction + std::stoi(match[5]),
                           prize_correction + std::stoi(match[6])}});
        start = match.suffix().first;
    }
    return result;
}


auto first(std::string s) -> void {
    std::string inp = slurp(s);
    std::vector<Input> i = parse_input(inp, 0);

    long tokens = 0;
    for (auto ai : i) {
        auto a = guldens(ai);
        if (a) {
            tokens += a.value();
        }
    }

    std::println("{}", tokens);
}


/* def solve_nk_no_lib(tx, ty, ax, ay, bx, by): */
/*     det = bx * ay - by * ax */
/*     if det == 0: */
/*         raise ValueError("System has no integer solution") */
/*     k = (tx * ay - ty * ax) / det */
/*     n = (tx - bx * k) / ax */
/*     return n, k */

auto is_int(double value) -> bool {
    return std::floor(value) == value;
}


auto guldens_lin(Input inp) -> std::optional<long> {
    double det = inp.b.first * inp.a.second - inp.b.second * inp.a.first;
    if (det == 0) {
        return {};
    }
    double k =
        (inp.prize.first * inp.a.second - inp.prize.second * inp.a.first) / det;
    double n = (inp.prize.first - inp.b.first * k) / inp.a.first;
    if (is_int(k) && is_int(n)) {
        return {static_cast<long>(k) * 1 + static_cast<long>(n) * 3};
    } else {
        return {};
    }
}


auto second(std::string s) -> void {
    std::string inp = slurp(s);
    std::vector<Input> i = parse_input(inp, 10000000000000);

    long tokens = 0;
    for (auto ai : i) {
        auto a = guldens_lin(ai);
        if (a) {
            tokens += a.value();
        }
    }

    std::println("{}", tokens);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/13.txt");
    first(filename);
    second(filename);
    return 0;
}
