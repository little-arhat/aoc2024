
#include "utils.hpp"


auto can_fold(const std::string& design,
              const std::unordered_set<std::string>& towels) -> bool {
    std::vector<bool> dp(design.size() + 1, false);
    dp[0] = true;
    for (size_t i = 1; i < design.size() + 1; i++) {
        for (size_t j = 0; j < i; j++) {
            if (dp[j] && towels.contains(design.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[design.size()];
}


auto second(std::string s) -> void {
    std::println("{}", s);
}


auto first(std::string s) -> void {
    std::unordered_set<std::string> towels;
    long possible = 0;
    read_lines(s, [&towels, &possible](std::string line) {
        if (towels.empty()) {
            std::string r{line};
            r.erase(std::remove(r.begin(), r.end(), ' '), r.end());
            auto v = r | std::views::split(',') |
                     std::views::transform([](auto&& range) {
                         return std::string(range.begin(), range.end());
                     });

            std::ranges::copy(v, std::inserter(towels, towels.end()));

        } else if (!line.empty()) {
            if (can_fold(line, towels)) {
                possible += 1;
            }
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
