
#include "utils.hpp"


enum class State : uint8_t {
    NONE,
    KEY,
    LOCK
};

static constexpr const std::array<int, 5> EMPTY{0, 0, 0, 0, 0};


auto first(std::string s) {
    std::vector<std::array<int, 5>> keys;
    std::vector<std::array<int, 5>> locks;
    State current_state = State::NONE;
    std::array<int, 5> current_object;


    read_lines(
        s,
        [&keys, &locks, &current_state, &current_object](std::string line) {
            if (line.empty()) {
                if (current_object != EMPTY) {
                    if (current_state == State::KEY) {
                        keys.push_back(current_object);
                    } else if (current_state == State::LOCK) {
                        locks.push_back(current_object);
                    }
                    current_object = EMPTY;
                }
                current_state = State::NONE;
                return;
            }
            if (current_state == State::NONE) {
                if (line[0] == '.') {
                    current_state = State::KEY;
                    current_object = {-1, -1, -1, -1, -1};
                    return;
                } else if (line[0] == '#') {
                    current_state = State::LOCK;
                    current_object = {0, 0, 0, 0, 0};
                    return;
                }
            }

            for (auto i = 0; i < 5; i++) {
                if (line[i] == '#') {
                    current_object[i] += 1;
                }
            }
        });
    if (current_object != EMPTY) {
        if (current_state == State::KEY) {
            keys.push_back(current_object);
        } else if (current_state == State::LOCK) {
            locks.push_back(current_object);
        }
    }

    int H = 5;
    long fit = 0;
    for (auto key : keys) {
        for (auto lock : locks) {
            bool cur_fit = true;
            for (auto i = 0; i < 5; i++) {
                cur_fit &= (H >= (lock[i] + key[i]));
            }
            if (cur_fit) {
                fit += 1;
            }
        }
    }

    std::println("{}", fit);
}


auto second(std::string s) {
    std::println("{}", s);
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/25.txt");
    first(filename);
    second(filename);
    return 0;
}
