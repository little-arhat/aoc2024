#include "utils.hpp"

auto first(std::string s) -> void {
    std::println("{}", s);
}


auto second(std::string s) -> void {
    std::println("{}", s);
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/17.txt");
    first(filename);
    second(filename);
    return 0;
}
