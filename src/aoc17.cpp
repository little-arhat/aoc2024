
#include "utils.hpp"


enum class Opcode : uint8_t {
    /* The adv instruction (opcode 0) performs division. The numerator is the
     * value in the A register. The denominator is found by raising 2 to the
     * power of the instruction's combo operand. (So, an operand of 2 would
     * divide A by 4 (2^2); an operand of 5 would divide A by 2^B.) The result
     * of the division operation is truncated to an integer and then written to
     * the A register. */
    ADV = 0,
    /* The bxl instruction (opcode 1) calculates the bitwise XOR of register B
     * and the instruction's literal operand, then stores the result in register
     * B. */
    BXL = 1,
    /* The bst instruction (opcode 2) calculates the value of its combo operand
     * modulo 8 (thereby keeping only its lowest 3 bits), then writes that value
     * to the B register. */
    BST = 2,
    /* The jnz instruction (opcode 3) does nothing if the A register is 0.
     * However, if the A register is not zero, it jumps by setting the
     * instruction pointer to the value of its literal operand; if this
     * instruction jumps, the instruction pointer is not increased by 2 after
     * this instruction.     */
    JNZ = 3,
    /* The bxc instruction (opcode 4) calculates the bitwise XOR of register B
     * and register C, then stores the result in register B. (For legacy
     * reasons, this instruction reads an operand but ignores it.) */
    BXC = 4,
    /* The out instruction (opcode 5) calculates the value of its combo operand
     * modulo 8, then outputs that value. (If a program outputs multiple values,
     * they are separated by commas.) */
    OUT = 5,
    /* The bdv instruction (opcode 6) works exactly like the adv instruction
     * except that the result is stored in the B register. (The numerator is
     * still read from the A register.) */
    BDV = 6,
    /* The cdv instruction (opcode 7) works exactly like the adv instruction
     * except that the result is stored in the C register. (The numerator is
     * still read from the A register.) */
    CDV = 7
};

auto opcode_to_string(Opcode opcode) -> std::string_view {
    switch (opcode) {
        case Opcode::ADV:
            return "adv";
        case Opcode::BXL:
            return "bxl";
        case Opcode::BST:
            return "bst";
        case Opcode::JNZ:
            return "jnz";
        case Opcode::BXC:
            return "bxc";
        case Opcode::OUT:
            return "out";
        case Opcode::BDV:
            return "bdv";
        case Opcode::CDV:
            return "cdv";
        default:
            return "unknown";
    }
}

auto int_to_opcode(uint8_t value) -> Opcode {
    if (value >= static_cast<int>(Opcode::ADV) &&
        value <= static_cast<int>(Opcode::CDV)) {
        return static_cast<Opcode>(value);
    }
    throw std::invalid_argument("Invalid opcode");
}


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


auto combo_operand(State& s, uint8_t operand) -> int {
    switch (operand) {
        case 0:
        case 1:
        case 2:
        case 3:
            return operand;
        case 4:
            return s.A;
        case 5:
            return s.B;
        case 6:
            return s.C;
        default:
            throw std::invalid_argument("Invalid combo operand");
    }
}


auto run(State& s, const std::vector<uint8_t>& prog) -> std::vector<int> {
    std::vector<int> r;
    while (s.pc < prog.size() - 1) {
        auto opcode = int_to_opcode(prog[s.pc]);
        auto operand = prog[s.pc + 1];
        std::println("Before; state: {}; opcode={}; operand={}",
                     s.to_string(),
                     opcode_to_string(opcode),
                     operand);

        switch (opcode) {
            case Opcode::ADV: {
                int combo_op = combo_operand(s, operand);
                int denom = std::pow(2, combo_op);
                int res = s.A / denom;
                s.A = res;
                break;
            }
            case Opcode::BXL: {
                int res = s.B ^ operand;
                s.B = res;
                break;
            }
            case Opcode::BST: {
                int combo_op = combo_operand(s, operand);
                int res = combo_op % 8;
                s.B = res;
                break;
            }
            case Opcode::JNZ: {
                if (s.A != 0) {
                    s.pc = operand;
                    // skip pc+=2
                    std::println("after op.. state: {}..jump", s.to_string());
                    continue;
                }
                break;
            }
            case Opcode::BXC: {
                int res = s.B ^ s.C;
                s.B = res;
                break;
            }
            case Opcode::OUT: {
                int combo_op = combo_operand(s, operand);
                r.push_back(combo_op);
                break;
            }
            case Opcode::BDV: {
                int combo_op = combo_operand(s, operand);
                int denom = std::pow(2, combo_op);
                int res = s.A / denom;
                s.B = res;
                break;
            }
            case Opcode::CDV: {
                int combo_op = combo_operand(s, operand);
                int denom = std::pow(2, combo_op);
                int res = s.A / denom;
                s.C = res;
                break;
            }
        }
        s.pc += 2;
        std::println("after op.. state: {}", s.to_string());
    }

    return r;
}


auto second(std::string s) -> void {
    std::println("{}", s);
}


auto first(std::string inp) -> void {
    State s{0, 0, 0, 0};
    std::vector<uint8_t> prog;

    read_lines(inp, [&s, &prog](std::string line) {
        std::string_view sv(line);

        if (sv.starts_with("Register A: ")) {
            auto value = sv.substr(12);
            std::from_chars(value.data(), value.data() + value.size(), s.A);
        } else if (sv.starts_with("Register B: ")) {
            auto value = sv.substr(12);
            std::from_chars(value.data(), value.data() + value.size(), s.B);
        } else if (sv.starts_with("Register C: ")) {
            auto value = sv.substr(12);
            std::from_chars(value.data(), value.data() + value.size(), s.C);
        } else if (sv.starts_with("Program: ")) {
            auto nums = sv.substr(8);
            for (auto n : split_gen<uint8_t>(nums, ',')) {
                prog.push_back(n);
            }
        }
    });

    std::println("Begin with state: {};", s.to_string());
    auto r = run(s, prog);

    std::println("end with state: {};", s.to_string());
    for (auto p : r) {
        std::print("{},", p);
    }
    std::println("");
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/17.txt");
    first(filename);
    //    second(filename);
    return 0;
}
