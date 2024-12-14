
#include <cmath>
#include <regex>
#include "utils.hpp"

// using pi = std::pair<long, long>;


auto wrap(int v, int bound) -> int {
    auto s = sign(v);
    auto vc = std::abs(v) % bound;
    return s > 0 || vc == 0 ? vc : bound - vc;
}


struct Robot {
    int x, y, vx, vy;
    auto to_string() const -> const std::string {
        return std::format("{{x={};y={}; vx={};vy={}}}", x, y, vx, vy);
    }

    auto move(int steps, int width, int height) const -> const Robot {
        return {wrap(x + steps * vx, width),
                wrap(y + (steps * vy), height),
                vx,
                vy};
    }
};

template <>
struct std::formatter<Robot> : std::formatter<std::string> {
    auto format(Robot i, format_context& ctx) const {
        return formatter<string>::format(i.to_string(), ctx);
    }
};


auto parse_input(const std::string& input) -> std::vector<Robot> {
    std::vector<Robot> robots;
    std::regex pattern(R"(\bp=(\d+),(\d+)\s+v=(-?\d+),(-?\d+)\b)");
    std::smatch match;
    std::istringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        if (std::regex_search(line, match, pattern)) {
            robots.push_back({std::stoi(match[1]),
                              std::stoi(match[2]),
                              std::stoi(match[3]),
                              std::stoi(match[4])});
        }
    }

    return robots;
}


auto clear(std::vector<std::string>& g, int width) -> void {
    for (auto& row : g) {
        row = std::string(width, ' ');
    }
}

auto print(std::vector<std::string>& g, int step) -> void {
    std::println("\n...STEP: {}....", step);
    for (const auto& row : g) {
        std::println("{}", row);
    }
    std::println();
}


auto second(std::string s) -> void {
    auto robots = parse_input(slurp(s));
    auto size = robots.back();
    robots.pop_back();

    // manual inspection of the log was performed
    std::string tmp("0000000000000000000000000000000");
    std::vector<std::string> grid(size.y, std::string(size.x, ' '));
    auto found = 0;
    for (int steps = 1; steps < 10000; steps += 1) {
        for (auto r : robots) {
            auto rm = r.move(steps, size.x, size.y);
            grid[rm.y][rm.x] = '0';
        }
        int count_lines = 0;
        for (const auto& row : grid) {
            if (row.find(tmp) != std::string::npos) {
                count_lines += 1;
            }
        }
        if (count_lines == 2) {
            print(grid, steps);
            found = steps;
            break;
        }
    }

    std::println("{}", found);
}


auto first(std::string s) -> void {
    auto robots = parse_input(slurp(s));
    auto size = robots.back();
    robots.pop_back();
    long hx = size.x / 2;
    long hy = size.y / 2;
    std::array<std::array<long, 2>, 2> quads = {{{{0, 0}}, {{0, 0}}}};
    int steps = 100;
    for (auto r : robots) {
        auto rm = r.move(steps, size.x, size.y);
        auto qx = sign(rm.x - hx);
        auto qy = sign(rm.y - hy);
        if (qx != 0 && qy != 0) {
            quads[std::max(0, qy)][std::max(0, qx)] += 1;
        }
    }
    long score = quads[0][0] * quads[0][1] * quads[1][0] * quads[1][1];
    std::println("{}", score);
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/14.txt");
    // first(filename);
    second(filename);
    return 0;
}
