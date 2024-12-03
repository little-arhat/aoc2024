
#include "utils.hpp"


auto is_safe(int a, int b, int last_diff) -> std::optional<int> {
    int diff = b - a;
    // std::println("diff: {}", diff);
    int ad = std::abs(diff);
    if (ad < 1 || ad > 3) {
        return {};
    } else if (last_diff == 0) {
        return {diff};
    } else if (sign(diff) != sign(last_diff)) {
        return {};
    }

    return {diff};
}

auto first(std::string input) -> void {
    std::ifstream file(input);
    int safe = 0;
    read_lines(file, [&safe](std::string line) {
        std::istringstream s(line);
        int a, b;
        int last_diff = 0;
        s >> a;
        while (s >> b) {
            if (auto ld = is_safe(a, b, last_diff)) {
                last_diff = ld.value();
            } else {
                return;
            }

            a = b;
        }
        safe++;
    });

    std::println("{}", safe);
}


template <typename T>
std::vector<T> exclude(const std::vector<T>& input, std::size_t index) {
    std::vector<T> result;
    result.reserve(input.size() - 1);
    result.insert(result.end(), input.begin(), input.begin() + index);
    result.insert(result.end(), input.begin() + index + 1, input.end());
    return result;
}


auto second(std::string input) -> void {
    std::ifstream file(input);
    int safe = 0;
    read_lines(file, [&safe](std::string line) {
        std::istringstream s(line);
        std::vector<int> v;

        int a, b;
        int last_diff = 0;
        bool broken = false;
        s >> a;
        v.push_back(a);
        while (s >> b) {
            v.push_back(b);
            if (broken) {
                continue;
            }
            if (auto ld = is_safe(a, b, last_diff)) {
                last_diff = ld.value();
            } else {
                broken = true;
            }

            a = b;
        }

        if (broken) {
            for (size_t i = 0; i < v.size(); i++) {
                int last_diff = 0;
                std::vector<int> r = exclude(v, i);
                bool good = true;
                for (const auto [a, b] :
                     std::views::zip(r, r | std::views::drop(1))) {
                    if (auto ld = is_safe(a, b, last_diff)) {
                        last_diff = ld.value();
                    } else {
                        good = false;
                        break;
                    }
                }
                if (good) {
                    safe++;
                    return;
                }
            }
        } else {
            safe++;
        }
    });

    std::println("{}", safe);
}

auto main(int argc, char* argv[]) -> int {
    std::string inp = aoc(argc, argv, "../inputs/2.txt");
    first(inp);
    second(inp);
    return 0;
}
