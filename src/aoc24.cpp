#include <deque>
#include "utils.hpp"


// operands
using ops = std::pair<std::string, std::string>;


enum class Op : uint8_t {
    OR,
    XOR,
    AND
};

auto op_from_string(std::string_view s) -> Op {
    if (s == "OR") {
        return Op::OR;
    } else if (s == "XOR") {
        return Op::XOR;
    } else {
        return Op::AND;
    }
}

auto run_op(Op op, uint8_t a, uint8_t b) -> uint8_t {
    switch (op) {
        case Op::OR:
            return (a | b) & 1;
        case Op::XOR:
            return (a ^ b) & 1;
        case Op::AND:
            return (a & b) & 1;
    }
}

auto op_to_string(Op op) -> std::string {
    switch (op) {
        case Op::OR:
            return "OR";
        case Op::XOR:
            return "XOR";
        case Op::AND:
            return "AND";
    }
}


auto second(std::string s) {
    std::println("{}", s);
}


auto first(std::string s) {
    std::unordered_map<std::string, uint8_t> state;
    // k computes -> values
    std::unordered_map<std::string, std::unordered_set<std::string>> conns;
    // k can be computed by -> (op, a, b)
    std::unordered_map<std::string, std::tuple<Op, std::string, std::string>>
        rules;
    // k depends on -> {a..}
    std::unordered_map<std::string, std::unordered_set<std::string>> deps;
    read_lines(s, [&state, &conns, &rules, &deps](std::string line) {
        if (line.find("->") != std::string::npos) {
            auto pos = line.find(" -> ");
            std::string expr(line.substr(0, pos));
            std::string out(line.substr(pos + 4));
            auto parts = std::views::split(line, std::string_view(" ")) |
                         std::views::transform([](auto&& range) {
                             return std::string(range.begin(), range.end());
                         });
            auto v = parts.begin();
            std::string l = *v;
            Op op = op_from_string(*std::next(v));
            std::string r = *std::next(v, 2);
            conns[l].insert(out);
            conns[r].insert(out);
            rules[out] = {op, l, r};
            deps[out].insert(l);
            deps[out].insert(r);
        } else if (line.find(':') != std::string::npos) {
            auto pos = line.find(": ");
            std::string gate(line.substr(0, pos));
            uint8_t value;
            std::from_chars(line.data() + pos + 2,
                            line.data() + line.size(),
                            value);
            state[gate] = value;
            conns[gate] = {};
        }
    });

    std::vector<std::string> gates;
    gates.reserve(conns.size());
    // 1. topo sort
    auto ks = std::views::keys(state);
    std::set<std::string> no_incoming_edges{ks.begin(), ks.end()};
    // we can modify conns & deps no prob, don't need it later
    while (!no_incoming_edges.empty()) {
        auto n = *no_incoming_edges.begin();
        no_incoming_edges.erase(n);
        gates.push_back(n);
        for (auto& m :
             get_or_default(conns, n, std::unordered_set<std::string>{})) {
            if (deps.contains(m)) {
                deps.at(m).erase(n);
                if (deps.at(m).empty()) {
                    no_incoming_edges.insert(m);
                }
            }
        }
    }

    // 2. evaluate
    unsigned long long z = 0;
    for (auto& gate : gates) {
        if (state.contains(gate)) {
            continue;  // already computed
        }
        auto [op, d1, d2] = rules[gate];
        auto d1v = state[d1];
        auto d2v = state[d2];
        auto res = run_op(op, d1v, d2v);
        state[gate] = res;
        if (gate.starts_with('z')) {
            uint8_t digit_pos = std::stoi(gate.substr(1, gate.size()));
            unsigned long long step = static_cast<unsigned long long>(res)
                                      << digit_pos;
            z += step;
        }
    }
    std::println("{}", z);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/24.txt");
    first(filename);
    second(filename);
    return 0;
}
