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

auto to_string(Op op) -> const str {
    switch (op) {
        case Op::OR:
            return "OR";
        case Op::XOR:
            return "XOR";
        case Op::AND:
            return "AND";
    }
}


auto to_string(const std::tuple<Op, str, str>& t) -> const str {
    auto [op, a, b] = t;
    return std::format("{} {} {}", a, to_string(op), b);
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


auto preview(const hashmap<str, uint8_t>& state, char mask) -> unsigned long {
    unsigned long res = 0;
    for (auto [k, v] : state) {
        if (k.starts_with(mask)) {
            uint8_t digit_pos = std::stoi(k.substr(1, k.size()));
            unsigned long step = static_cast<unsigned long>(v) << digit_pos;
            res += step;
        }
    }

    return res;
}


template <size_t N>
auto load(unsigned long x, unsigned long y) -> hashmap<str, uint8_t> {
    std::bitset<N> bitsx(x);
    std::bitset<N> bitsy(y);

    hashmap<str, uint8_t> s;
    for (size_t i = 0; i < N; ++i) {
        s[std::format("x{:02}", i)] = bitsx[i];
        s[std::format("y{:02}", i)] = bitsy[i];
    }

    return s;
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


auto eval_full(const hashmap<str, uint8_t>& precomputed,
               const hashmap<str, std::tuple<Op, str, str>>& rules)
    -> std::pair<unsigned long, hashmap<str, uint8_t>> {
    std::vector<std::string> gates = topo(precomputed, rules);
    hashmap<str, uint8_t> state{precomputed};
    auto z = eval(state, rules, gates);
    return {z, state};
}


auto swap_outs(const hashmap<str, std::tuple<Op, str, str>>& rules,
               std::vector<std::pair<str, str>> pairs)
    -> hashmap<str, std::tuple<Op, str, str>> {
    hashmap<str, std::tuple<Op, str, str>> new_rules{rules};
    for (auto [a, b] : pairs) {
        auto old_a = new_rules[a];
        auto old_b = new_rules[b];
        new_rules[a] = old_b;
        new_rules[b] = old_a;
    }

    return new_rules;
}


constexpr auto nth_bit(unsigned long value, size_t n) -> uint8_t {
    return static_cast<uint8_t>((value >> n) & 1);
}


struct NCRes {
    hashmap<int, hashmap<str, int>> next_carry;
    hashset<int> next_used;
};

auto compute_next_carry(int i,
                        const hashmap<int, hashmap<str, int>>& carry,
                        const hashmap<str, std::tuple<Op, str, str>>& rules,
                        const hashmap<str, uint8_t>& precomputed)
    -> std::optional<NCRes> {
    auto xi = std::format("x{:02}", i);
    auto yi = std::format("y{:02}", i);
    auto zi = std::format("z{:02}", i);

    hashmap<int, hashmap<str, int>> next_carry;
    hashset<int> next_used;

    for (int cv = 0; cv <= 1; ++cv) {
        if (!carry.contains(cv))
            continue;

        auto carry_map = carry.at(cv);
        for (int xv = 0; xv <= 1; ++xv) {
            for (int yv = 0; yv <= 1; ++yv) {
                auto cur = carry_map;
                cur[xi] = xv;
                cur[yi] = yv;

                hashmap<str, uint8_t> state(precomputed.begin(),
                                            precomputed.end());
                for (const auto& [key, value] : cur) {
                    state[key] = static_cast<uint8_t>(value);
                }

                // Continue debugging and applying idia from eliz to my code
                // use evaluated & rules to find value equiv to:
                // if (c.dest in cur) {
                //     check(cur[c.dest] == curRes)
                // } else {
                //     cur[c.dest] = curRes
                //             nextUsed += codeIndex
                //             addNext(c.dest)
                //         changes = true
                // }

                auto [z, evaluated] = eval_full(state, rules);
                unsigned long checkz = preview(evaluated, 'z');
                int zv = static_cast<int>(nth_bit(z, i));
                int sum = xv + yv + cv;
                std::println("\tchz: {}, evaluated: {}", checkz, evaluated);
                std::println("z={}; xv={}; yv={}; cv={}; s1={}; cur={}",
                             z,
                             xv,
                             yv,
                             cv,
                             (sum & 1),
                             cur);

                if (zv != (sum & 1))
                    return std::nullopt;

                int nc = sum >> 1;
                std::println("NC:{}; contains:{}; cur={}",
                             nc,
                             next_carry.contains(nc),
                             cur);
                if (next_carry.contains(nc)) {
                    for (const auto& [key, val] : next_carry[nc]) {
                        if (cur.contains(key) && cur[key] != val) {
                            cur.erase(key);
                        }
                    }
                }

                next_carry[nc] = cur;
            }
        }
    }

    return NCRes{next_carry, next_used};
}


auto second(std::string s) {
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

    hashmap<int, hashmap<str, int>> carry{};
    carry[0] = {};

    //  next carry is NCRes(nextCarry={
    //       0={dsr=0, y00=0, z00=1},
    //       1={dsr=1, y00=1, x00=1, z00=0}},
    //       nextUsed=[113, 188]) for 0 & {0={}}
    if (auto nc = compute_next_carry(0, carry, rules, state)) {
        std::println("{}; {}", nc->next_used, nc->next_carry);
    }

    // auto ns = load<45>(68585520640, 34118178995231);
    // auto z0 = eval(ns, rules);
    // auto nr = swap_outs(
    //     rules,
    //     {{"z20", "nhs"}, {"wrc", "z34"}, {"ddn", "kqh"}, {"z09", "nnf"}});
    // auto z1 = eval(ns, nr);
    // std::println("{} vs {}", z0, z1);
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
    // first(filename);
    second(filename);
    return 0;
}
