
#include "utils.hpp"


constexpr auto concat(long a, long b) -> long {
    auto b_str = std::to_string(b);
    return std::stol(std::to_string(a) + b_str);
}


auto first(std::string s) -> void {
    std::ifstream file(s);
    long sum = 0;
    read_lines(file, [&sum](std::string line) {
        auto colon_pos = line.find(':');
        long num;
        std::from_chars(line.data(), line.data() + colon_pos, num);
        auto nums = line.substr(colon_pos + 2) | std::views::split(' ') |
                    std::views::transform([](auto&& part) {
                        long n;
                        std::string s(part.begin(), part.end());
                        std::from_chars(s.data(), s.data() + s.size(), n);
                        return n;
                    });
        std::set<long> options;
        for (auto n : nums) {
            std::set<long> tmp;
            for (auto o : options) {
                tmp.insert(o + n);
                tmp.insert(o * n);
            }
            if (options.empty()) {
                tmp.insert(n);
            }
            options = tmp;
        }
        if (options.contains(num)) {
            sum += num;
        }
    });

    std::println("{}", sum);
}


auto second(std::string s) -> void {
    std::ifstream file(s);
    long sum = 0;
    read_lines(file, [&sum](std::string line) {
        auto colon_pos = line.find(':');
        long num;
        std::from_chars(line.data(), line.data() + colon_pos, num);
        auto nums = line.substr(colon_pos + 2) | std::views::split(' ') |
                    std::views::transform([](auto&& part) {
                        long n;
                        std::string s(part.begin(), part.end());
                        std::from_chars(s.data(), s.data() + s.size(), n);
                        return n;
                    });
        std::set<long> options;
        for (auto n : nums) {
            std::set<long> tmp;
            for (auto o : options) {
                tmp.insert(o + n);
                tmp.insert(o * n);
                tmp.insert(concat(o, n));
            }
            if (options.empty()) {
                tmp.insert(n);
            }
            options = tmp;
        }
        if (options.contains(num)) {
            sum += num;
        }
    });

    std::println("{}", sum);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/7.txt");
    first(filename);
    second(filename);
    return 0;
}
