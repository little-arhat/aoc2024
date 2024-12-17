
#include "utils.hpp"


struct State {
    long A, B, C;
    unsigned int pc;
    auto to_string() const -> const std::string {
        return std::format("State{{A={};B={};C={};pc={}}}", A, B, C, pc);
    }
};

template <>
struct std::formatter<State> : std::formatter<std::string> {
    auto format(const State& i, std::format_context& ctx) const {
        return std::formatter<std::string>::format(i.to_string(), ctx);
    }
};


auto second(std::string s) -> void {
    std::println("{}", s);
}


auto first(std::string inp) -> void {
    State s{0, 0, 0, 0};
    std::vector<int> prog;

    read_lines(inp, [&s, &prog](std::string line) {
        std::string_view sv(line);

        if (sv.starts_with("Register A:")) {
            auto value = sv.substr(11);
            std::from_chars(value.data(), value.data() + value.size(), s.A);
        } else if (sv.starts_with("Register B:")) {
            auto value = sv.substr(11);
            std::from_chars(value.data(), value.data() + value.size(), s.B);
        } else if (sv.starts_with("Register C:")) {
            auto value = sv.substr(11);
            std::from_chars(value.data(), value.data() + value.size(), s.C);
        } else if (sv.starts_with("Program: ")) {
            auto nums = sv.substr(8);
            for (auto n : split_gen<int>(nums, ',')) {
                prog.push_back(n);
            }
        }
    });

    // c++ is shit; this clang doesn't do prints properly
    std::println("State: {};", s.to_string());
    for (auto p : prog) {
        std::print("{}, ", p);
    }
    std::println("");
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/17.txt");
    first(filename);
    second(filename);
    return 0;
}
