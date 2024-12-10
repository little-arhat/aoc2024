
#include "utils.hpp"


auto first(std::string s) -> void {
    std::ifstream file(s);

    bool rules = true;
    std::unordered_map<int, std::unordered_set<int>> forbidden;

    int sum = 0;
    read_lines(file, [&forbidden, &rules, &sum](std::string line) {
        if (line.size() == 0) {
            rules = false;
            return;
        }
        if (rules) {
            auto [first, second] = to_pair<int>(line, '|');
            forbidden[second].insert(first);
        } else {
            auto update = split<int>(line, ',');
            bool valid = true;
            std::unordered_set<int> f;
            for (auto u : update) {
                if (f.contains(u)) {
                    valid = false;
                    break;
                }
                auto f_for_u = forbidden[u];
                f.insert(f_for_u.begin(), f_for_u.end());
            }

            if (valid) {
                sum += update[update.size() / 2];
            }
        }
    });

    std::println("{}", sum);
}


auto second(std::string s) -> void {
    std::ifstream file(s);

    bool rules = true;
    std::unordered_map<int, std::unordered_set<int>> forbidden;

    int sum = 0;
    read_lines(file, [&forbidden, &rules, &sum](std::string line) {
        if (line.size() == 0) {
            rules = false;
            return;
        }
        if (rules) {
            auto [first, second] = to_pair<int>(line, '|');
            forbidden[second].insert(first);
        } else {
            bool valid = true;
            std::vector<int> ord;
            std::unordered_set<int> f;

            auto update = split_gen<int>(line, ',');
            for (auto u : update) {
                auto f_for_u = forbidden[u];
                if (f.contains(u)) {
                    valid = false;
                }

                if (valid) {
                    f.insert(f_for_u.begin(), f_for_u.end());
                    ord.push_back(u);
                } else {
                    bool inserted = false;
                    for (auto it = ord.begin(); it != ord.end(); ++it) {
                        auto cu = *it;
                        if (!f_for_u.contains(cu)) {
                            ord.insert(it, u);
                            inserted = true;
                            break;
                        }
                    }
                    if (!inserted) {
                        ord.push_back(u);
                    }
                }
            }

            if (!valid) {
                sum += ord[ord.size() / 2];
            }
        }
    });

    std::println("{}", sum);
}


auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/5.txt");
    first(filename);
    second(filename);
    return 0;
}
