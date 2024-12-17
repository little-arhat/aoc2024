
#include <numeric>
#include "utils.hpp"


/// fast log10
static const std::array<unsigned long, 20> ten_to = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
};

static const std::array<unsigned char, 65> GUESS = {
    0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,
    5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10,
    10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15,
    15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19};


constexpr auto digits2(unsigned long x) -> unsigned long {
    return x ? 64 - __builtin_clzl(x) : 0;
}

constexpr auto digits10(unsigned long x) -> unsigned int {
    unsigned int digits = GUESS[digits2(x)];
    return digits + (x >= ten_to[digits]);
}


constexpr auto split_num(unsigned long n) -> std::pair<long, long> {
    auto d = digits10(n);
    unsigned long divisor = ten_to[d / 2];
    return {n / divisor, n % divisor};
}

auto evolve(int it, std::unordered_map<unsigned long, unsigned long> g)
    -> std::unordered_map<unsigned long, unsigned long> {
    auto round = g;
    auto next = g;
    for (auto i = 0; i < it; i++) {
        next.clear();
        for (auto [k, v] : round) {
            if (k == 0) {
                next[1] += v;
            } else if ((digits10(k) & 1) == 0) {
                auto [f, s] = split_num(k);
                next[f] += v;
                next[s] += v;
            } else {
                next[k * 2024] += v;
            }
        }
        round = next;
    }

    return round;
}


auto first(std::string inp) -> void {
    auto content = slurp(inp);
    std::unordered_map<unsigned long, unsigned long> g;
    for (auto i : split_gen<unsigned long>(content, ' ')) {
        g[i] = 1;
    }
    auto a = evolve(25, g);
    long sum =
        std::transform_reduce(a.begin(),
                              a.end(),
                              0,
                              std::plus<>(),
                              [](const auto& pair) { return pair.second; });

    std::println("{}", sum);
}


auto second(std::string inp) -> void {
    auto content = slurp(inp);
    std::unordered_map<unsigned long, unsigned long> g;
    for (auto i : split_gen<unsigned long>(content, ' ')) {
        g[i] = 1;
    }
    auto a = evolve(75, g);
    unsigned long sum = std::accumulate(
        std::begin(a),
        std::end(a),
        0l,
        [](const unsigned long previous,
           const std::pair<const unsigned long, unsigned long>& p) {
            return previous + p.second;
        });
    std::println("{}", sum);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/11.txt");
    first(filename);
    second(filename);
    return 0;
}
