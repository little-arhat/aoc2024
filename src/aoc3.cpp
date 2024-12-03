
#include <algorithm>
#include <fstream>
#include <numeric>
#include <print>
#include <ranges>
#include <regex>

auto main(int argc, char* argv[]) -> int {
    std::string inp("../inputs/3.txt");
    if (argc > 1) {
        std::string arg1(argv[1]);
        inp = arg1;
    }
    std::ifstream file(inp);
    std::string s((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());

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
    return 0;
}
