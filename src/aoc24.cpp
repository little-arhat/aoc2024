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

auto op_to_string(Op op) -> str {
    switch (op) {
        case Op::OR:
            return "OR";
        case Op::XOR:
            return "XOR";
        case Op::AND:
            return "AND";
    }
}


auto eval(hashmap<str, uint8_t> state,
          const hashmap<str, std::tuple<Op, str, str>>& rules,
          const std::vector<str>& gates) -> unsigned long {
    unsigned long z = 0;
    for (auto& gate : gates) {
        if (state.contains(gate)) {
            continue;  // already computed
        }
        auto [op, d1, d2] = rules.at(gate);
        auto d1v = state[d1];
        auto d2v = state[d2];
        auto res = run_op(op, d1v, d2v);
        state[gate] = res;
        if (gate.starts_with('z')) {
            uint8_t digit_pos = std::stoi(gate.substr(1, gate.size()));
            unsigned long step = static_cast<unsigned long>(res) << digit_pos;
            z += step;
        }
    }

    return z;
}


auto topo(const hashmap<str, hashset<str>>& incoming) -> std::vector<str> {
    hashmap<str, hashset<str>> outgoing;
    hashset<str> no_incoming_edges;
    hashmap<str, hashset<str>> deps;
    for (auto& [n, dependencies] : incoming) {
        if (dependencies.empty()) {
            no_incoming_edges.insert(n);
        } else {
            for (auto nn : dependencies) {
                deps[n].insert(nn);
                outgoing[nn].insert(n);
            }
        }
    }

    std::vector<str> result;
    result.reserve(incoming.size());
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


auto second(std::string s) {
    std::println("{}", s);
}


auto first(std::string s) {
    hashmap<std::string, uint8_t> state;
    // k can be computed by -> (op, a, b)
    hashmap<str, std::tuple<Op, str, str>> rules;
    // k depends on -> {a..}
    hashmap<std::string, hashset<std::string>> deps;
    read_lines(s, [&state, &rules, &deps](std::string line) {
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
            deps[gate] = {};
        }
    });


    // 1. topo sort
    std::vector<std::string> gates = topo(deps);
    // 2. evaluate
    unsigned long long z = eval(state, rules, gates);
    std::println("{}", z);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/24.txt");
    first(filename);
    second(filename);
    return 0;
}
