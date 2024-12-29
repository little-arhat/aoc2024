#include <algorithm>
#include "utils.hpp"


using str = std::string;
template <typename K, typename V>
using hashmap = std::unordered_map<K, V>;
template <typename V>
using hashset = std::unordered_set<V>;


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


auto topo(const hashmap<str, uint8_t>& precomputed,
          const hashmap<str, std::tuple<Op, str, str>>& rules)
    -> std::vector<str> {
    auto ks = std::views::keys(precomputed);
    hashset<str> no_incoming_edges{ks.begin(), ks.end()};

    hashmap<str, hashset<str>> deps;
    hashmap<str, hashset<str>> outgoing;
    for (auto& [n, dependencies] : rules) {
        auto [op, l, r] = dependencies;
        deps[n].insert(l);
        outgoing[l].insert(n);

        deps[n].insert(r);
        outgoing[r].insert(n);
    }

    std::vector<str> result;
    result.reserve(rules.size() + precomputed.size());
    while (!no_incoming_edges.empty()) {
        auto n = *no_incoming_edges.begin();
        no_incoming_edges.erase(n);
        result.push_back(n);
        for (auto& m :
             get_or_default(outgoing, n, std::unordered_set<std::string>{})) {
            if (deps.contains(m)) {
                deps.at(m).erase(n);
                if (deps.at(m).empty()) {
                    no_incoming_edges.insert(m);
                }
            }
        }
    }

    return result;
}


auto eval(hashmap<str, uint8_t>& state,
          const hashmap<str, std::tuple<Op, str, str>>& rules,
          const std::vector<str>& gates) -> unsigned long {
    unsigned long z = 0;
    for (auto& gate : gates) {
        if (state.contains(gate)) {
            continue;  // already computed
        }
        auto [op, d1, d2] = rules.at(gate);
        auto res = run_op(op, state[d1], state[d2]);
        state[gate] = res;
        if (gate.starts_with('z')) {
            uint8_t digit_pos = std::stoi(gate.substr(1, gate.size()));
            unsigned long step = static_cast<unsigned long>(res) << digit_pos;
            z += step;
        }
    }

    return z;
}


auto eval(const hashmap<str, uint8_t>& precomputed,
          const hashmap<str, std::tuple<Op, str, str>>& rules)
    -> unsigned long {
    std::vector<std::string> gates = topo(precomputed, rules);
    hashmap<str, uint8_t> state{precomputed};
    return eval(state, rules, gates);
}


constexpr auto is_in_gate(const std::string_view& s) -> bool {
    return s[0] == 'x' || s[0] == 'y';
}


constexpr auto is_out_gate(const std::string_view& s) -> bool {
    return s[0] == 'z';
}


constexpr auto is_io_gate(const std::string_view& s) -> bool {
    return is_out_gate(s) || is_in_gate(s);
}


auto second(std::string s) {
    // k can be computed by -> (op, a, b)
    hashmap<str, std::tuple<Op, str, str>> rules;
    read_lines(s, [&rules](std::string line) {
        if (line.find("->") != std::string::npos) {
            auto parts = split(line, ' ');
            str l = parts[0];
            Op op = op_from_string(parts[1]);
            str r = parts[2];
            str out = parts[4];
            rules[out] = {op, l, r};
        }
    });

    auto last_out = *std::ranges::max_element(std::views::keys(rules));
    auto first_in = "x00";
    std::set<str> swapped;

    // https://en.wikipedia.org/wiki/Adder_(electronics)#Ripple-carry_adder
    for (auto& [gate, rule] : rules) {
        auto [op, a, b] = rule;
        if (op == Op::XOR) {
            if (!is_io_gate(gate) && !is_io_gate(a) && !is_io_gate(b)) {
                swapped.insert(gate);
            }

            for (auto& [other_gate, other_rule] : rules) {
                auto [other_op, other_a, other_b] = other_rule;
                if ((gate == other_a || gate == other_b) &&
                    other_op == Op::OR) {
                    swapped.insert(gate);
                }
            }
        } else if (is_out_gate(gate) && gate != last_out) {
            swapped.insert(gate);
        } else if (op == Op::AND && (a != first_in && b != first_in)) {
            for (auto& [other_gate, other_rule] : rules) {
                auto [other_op, other_a, other_b] = other_rule;
                if ((gate == other_a || gate == other_b) &&
                    other_op != Op::OR) {
                    swapped.insert(gate);
                }
            }
        }
    }

    std::println("{}", join_str(swapped, ','));
}


auto first(std::string s) {
    hashmap<std::string, uint8_t> state;
    // k can be computed by -> (op, a, b)
    hashmap<str, std::tuple<Op, str, str>> rules;
    read_lines(s, [&state, &rules](std::string line) {
        if (line.find("->") != std::string::npos) {
            auto parts = split(line, ' ');
            str l = parts[0];
            Op op = op_from_string(parts[1]);
            str r = parts[2];
            str out = parts[4];
            rules[out] = {op, l, r};
        } else if (line.find(':') != std::string::npos) {
            auto pos = line.find(": ");
            std::string gate(line.substr(0, pos));
            uint8_t value;
            std::from_chars(line.data() + pos + 2,
                            line.data() + line.size(),
                            value);
            state[gate] = value;
        }
    });

    std::println("{}", eval(state, rules));
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/24.txt");
    first(filename);
    second(filename);
    return 0;
}
