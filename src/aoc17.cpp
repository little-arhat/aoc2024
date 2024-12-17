
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
        if (line.starts_with("Register A:")) {
            sscanf(line.c_str(), "Register A: %ld", &s.A);
        } else if (line.starts_with("Register B:")) {
            sscanf(line.c_str(), "Register B: %ld", &s.B);
        } else if (line.starts_with("Register C:")) {
            sscanf(line.c_str(), "Register C: %ld", &s.C);
        } else if (line.starts_with("Program:")) {
            std::string nums = line.substr(9);
            std::istringstream iss(nums);
            int value;
            while (iss >> value) {
                prog.push_back(value);
                if (iss.peek() == ',')
                    iss.ignore();
            }
        }
    });

    // c++ is shit; this clang doesn't do prints properly
    std::println("State: {};", s.to_string());
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/17.txt");
    first(filename);
    second(filename);
    return 0;
}
