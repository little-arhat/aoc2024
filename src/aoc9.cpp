
#include "utils.hpp"


auto repeat(char c, std::size_t n) -> std::string {
    return std::string(n, c);
}


auto repeat(std::string s, std::size_t n) -> std::string {
    std::string result;
    result.reserve(s.size() * n);
    for (std::size_t i = 0; i < n; ++i)
        result += s;
    return result;
}


// opt: can be merged?
struct Block {
    int id, begin, size;

    auto to_string() const -> const std::string {
        return std::format("B[id={}; beg={}; size={}]", id, begin, size);
    }
};

template <>
struct std::formatter<Block> : std::formatter<std::string> {
    auto format(Block b, format_context& ctx) const {
        return formatter<string>::format(b.to_string(), ctx);
    }
};


struct Span {
    int begin, end, free_space;

    auto to_string() const -> const std::string {
        return std::format("S[beg={}; end={}; free={}]",
                           begin,
                           end,
                           free_space);
    }
};

template <>
struct std::formatter<Span> : std::formatter<std::string> {
    auto format(Span s, format_context& ctx) const {
        return formatter<string>::format(s.to_string(), ctx);
    }
};


auto first(std::string s) -> void {
    auto input = slurp(s);
    long checksum = 0;

    int num_files = (input.size() / 2) - 1;
    int ns = input.size();
    int tail = input.size() - 2;
    int tail_id = num_files;
    int head = 0;
    int head_id = 0;
    int free_space = 0;
    long pos = 0;

    while (head < tail) {
        if (free_space == 0) {
            int head_file_size = digit(input[head]);
            free_space = head < ns - 2 ? digit(input[head + 1]) : 0;

            for (int i = 0; i < head_file_size; i++) {
                checksum += (head_id * pos);
                pos += 1;
            }
        }
        int tail_file_size = digit(input[tail]);
        int can_move = std::min(tail_file_size, free_space);
        if (can_move > 0) {
            free_space -= can_move;
            input[tail] = ('0' + (tail_file_size - can_move));
            for (int i = 0; i < can_move; i++) {
                checksum += (tail_id * pos);
                pos += 1;
            }
        }
        // moved all
        if (tail_file_size - can_move == 0) {
            tail -= 2;
            tail_id -= 1;
        }
        if (free_space == 0) {
            head += 2;
            head_id += 1;
        }
    }
    auto rest = digit(input[tail]);
    if (rest > 0) {
        for (int i = 0; i < rest; i++) {
            checksum += (tail_id * pos);
            pos += 1;
        }
    }
    std::println("{}", checksum);
}


auto second(std::string s) -> void {
    auto input = slurp(s);
    size_t ns = input.size();

    // id -> span
    // opt: Btree<free_space, vec<Span>>
    std::vector<Span> spans;
    std::vector<Block> original;
    int cursor = 0;
    for (size_t i = 0; i < input.size(); i += 2) {
        int file_id = i / 2;
        int size = digit(input[i]);
        int free_space = i < ns - 2 ? digit(input[i + 1]) : 0;
        int total = size + free_space;
        Span s{cursor, cursor + total, free_space};
        spans.push_back(s);
        Block b{file_id, cursor, size};
        original.push_back(b);
        cursor += total;
    }

    // unsorted
    std::vector<Block> moved;
    for (int i = original.size() - 1; i >= 0; --i) {
        auto b = original[i];
        bool m = false;
        for (auto left : original) {
            if (left.id == b.id) {
                break;
            }
            Span* containing_span = &spans[left.id];
            if (containing_span->free_space >= b.size) {
                auto fs = containing_span->free_space;
                auto mpos = containing_span->end - fs;
                // can move
                containing_span->free_space -= b.size;
                Block mb{b.id, mpos, b.size};
                moved.push_back(mb);
                m = true;
                break;
            }
        }
        if (!m) {
            moved.push_back(b);
        }
    }

    long checksum = 0;
    for (auto mb : moved) {
        for (auto i = mb.begin; i < mb.begin + mb.size; i++) {
            checksum += (i * mb.id);
        }
    }
    std::println("{}", checksum);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/9.txt");
    first(filename);
    second(filename);
    return 0;
}
