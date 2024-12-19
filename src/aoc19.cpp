
#include <utility>
#include "utils.hpp"


template <typename H, typename E>
auto can_fold(const std::string_view& design,
              const std::unordered_set<std::string, H, E>& towels) -> long {
    std::vector<long> dp(design.size() + 1, 0);
    dp[0] = 1;
    for (size_t i = 1; i < design.size() + 1; i++) {
        for (size_t j = 0; j < i; j++) {
            if (towels.contains(design.substr(j, i - j))) {
                dp[i] += dp[j];
            }
        }
    }
    return dp[design.size()];
}


auto second(std::string s) -> void {
    std::unordered_set<std::string, universal_string_hash, std::equal_to<>>
        towels;
    long possible = 0;
    read_lines(s, [&towels, &possible](std::string line) {
        if (towels.empty()) {
            auto v = std::views::split(line, std::string_view(", ")) |
                     std::views::transform([](auto&& range) {
                         return std::string(range.begin(), range.end());
                     });
            towels.insert(v.begin(), v.end());
        } else if (!line.empty()) {
            possible += can_fold(line, towels);
        }
    });

    std::println("{}", possible);
}


auto first(std::string s) -> void {
    std::unordered_set<std::string, universal_string_hash, std::equal_to<>>
        towels;
    long possible = 0;
    read_lines(s, [&towels, &possible](std::string line) {
        if (towels.empty()) {
            auto v = std::views::split(line, std::string_view(", ")) |
                     std::views::transform([](auto&& range) {
                         return std::string(range.begin(), range.end());
                     });
            towels.insert(v.begin(), v.end());
        } else if (!line.empty()) {
            possible += std::cmp_greater(can_fold(line, towels), 0);
        }
    });

    std::println("{}", possible);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/19.txt");
    first(filename);
    second(filename);
    return 0;
}
