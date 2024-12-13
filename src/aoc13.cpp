#include "utils.hpp"

auto first(std::string s) -> void {
    std::string inp = slurp(s);
    std::println("{}", inp.size());
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
