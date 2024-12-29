#include <bitset>
#include "utils.hpp"


auto mixprune(unsigned long secret, unsigned long result) -> unsigned long {
    secret ^= result;
    secret &= 0xFFFFFF;
    return secret;
}


auto evolve(unsigned long input) -> unsigned long {
    auto secret = mixprune(input, input << 6);
    secret = mixprune(secret, secret >> 5);
    secret = mixprune(secret, secret << 11);
    return secret;
}


auto evolve(unsigned long input, int steps) -> unsigned long {
    auto s = input;
    for (auto n = 0; n < steps; n++) {
        s = evolve(s);
    }
    return s;
}


auto price(unsigned long secret) -> int {
    return secret % 10;
}


// -> (price, change)
// not used yet, as I went with gorilla brute force, not orangutan smarts
auto seq(unsigned long input, int steps) -> std::vector<std::pair<int, int>> {
    std::vector<std::pair<int, int>> r;
    r.reserve(steps);
    auto s = input;
    auto last_price = price(s);
    for (auto n = 0; n < steps; n++) {
        s = evolve(s);
        auto new_price = price(s);
        r.push_back({new_price, new_price - last_price});
        last_price = new_price;
    }
    return r;
}


auto shift_left(std::array<int, 4> arr, int value) -> std::array<int, 4> {
    std::array<int, 4> n;
    for (std::size_t i = 0; i < arr.size() - 1; ++i) {
        n[i] = arr[i + 1];
    }
    n.back() = value;
    return n;
}


// (-4, -3, -2, -1) diffs => price
auto mapped(unsigned long input, int steps)
    -> std::unordered_map<std::array<int, 4>, int, array_hash> {
    std::unordered_map<std::array<int, 4>, int, array_hash> m;
    m.reserve(steps - 4);

    auto s = input;
    auto last_price = price(s);
    std::array<int, 4> last4{INT_MIN, INT_MIN, INT_MIN, INT_MIN};
    for (auto n = 0; n < steps; n++) {
        s = evolve(s);
        auto new_price = price(s);
        auto diff = new_price - last_price;
        last_price = new_price;
        last4 = shift_left(last4, diff);
        if (n >= 3) {
            // store only first
            if (!m.contains(last4)) {
                m[last4] = new_price;
            }
        }
    }
    return m;
}


auto second(std::string s) {
    std::vector<std::unordered_map<std::array<int, 4>, int, array_hash>> buyers;
    std::unordered_set<std::array<int, 4>, array_hash> unique_diffs;

    read_lines<int>(s, [&buyers, &unique_diffs](int secret) {
        auto m = mapped(secret, 2000);
        buyers.push_back(m);
        for (auto const& key : std::views::keys(m)) {
            unique_diffs.insert(key);
        }
    });

    unsigned long best_bananas = 0;
    for (auto const& diff : unique_diffs) {
        unsigned long bananas = 0;
        for (auto const& buyer : buyers) {
            bananas += get_or_default(buyer, diff, 0);
        }
        best_bananas = std::max(bananas, best_bananas);
    }

    std::println("{}", best_bananas);
}


auto first(std::string s) {
    unsigned long sum = 0;
    read_lines<int>(s, [&sum](int secret) { sum += evolve(secret, 2000); });
    std::println("{}", sum);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/22.txt");
    first(filename);
    second(filename);
    return 0;
}
