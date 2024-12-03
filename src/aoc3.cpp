
#include <regex>

#include "utils.hpp"


auto first(std::string s) -> void {
    std::regex re("mul\\((\\d+),\\s*(\\d+)\\)");
    auto sum = 0;
    for (std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), re);
         i != std::sregex_iterator();
         ++i) {
        std::smatch m = *i;
        auto a = std::stoi(m.str(1));
        auto b = std::stoi(m.str(2));
        sum += (a * b);
    }

    std::println("{}", sum);
}


auto second(std::string s) -> void {
    std::regex re(R"(((?:do\(\)|don't\(\)))|mul\((\d+),(\d+)\))");

    auto sum = 0;
    bool next_disabled = false;
    for (std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), re);
         i != std::sregex_iterator();
         ++i) {
        std::smatch m = *i;
        if (m.str(0) == "do()") {
            next_disabled = false;
        } else if (m.str(0) == "don't()") {
            next_disabled = true;
        } else if (!next_disabled) {
            auto a = std::stoi(m.str(2));
            auto b = std::stoi(m.str(3));
            sum += (a * b);
        }
    }

    std::println("{}", sum);
}


auto main(int argc, char* argv[]) -> int {
    std::string s = slurp(aoc(argc, argv, "../inputs/3.txt"));
    first(s);
    second(s);
    return 0;
}
