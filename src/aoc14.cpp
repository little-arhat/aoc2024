
#include <algorithm>
#include <cmath>
#include <regex>
#include "utils.hpp"


auto mean_var(double new_val,
              double prev_mean,
              double prev_var,
              size_t cur_count) -> std::pair<double, double> {
    if (cur_count == 1) {
        return {new_val, 0.0};
    }
    double new_mean = prev_mean + (new_val - prev_mean) / cur_count;
    double new_var = prev_var + ((new_val - prev_mean) * (new_val - new_mean));
    return {new_mean, new_var};
}


auto mod_pow(long base, long exp, long mod) -> long {
    long result = 1;
    base = base % mod;
    if (exp < 0) {
        exp = -exp;
        base = mod_pow(base, mod - 2, mod);
    }
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}


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


auto second(std::string s) -> void {
    auto robots = parse_input(slurp(s));
    auto size = robots.back();
    robots.pop_back();

    long best_x, best_x_var, best_y, best_y_var;
    best_y_var = LONG_MAX;
    best_x_var = LONG_MAX;
    for (int steps = 1; steps < std::max(size.x, size.y); steps += 1) {
        double meanx, meany, xvar, yvar;
        size_t c = 0;
        for (auto r : robots) {
            c++;
            auto rm = r.move(steps, size.x, size.y);
            std::tie(meanx, xvar) = mean_var(rm.x, meanx, xvar, c);
            std::tie(meany, yvar) = mean_var(rm.y, meany, yvar, c);
        }
        if (xvar < best_x_var) {
            best_x_var = xvar;
            best_x = steps;
        }
        if (yvar < best_y_var) {
            best_y_var = yvar;
            best_y = steps;
        }
    }
    // smarts:
    // https://old.reddit.com/r/adventofcode/comments/1hdvhvu/2024_day_14_solutions/m1zws1g/
    auto found =
        best_x +
        ((mod_pow(size.x, -1, size.y) * (best_y - best_x)) % size.y) * size.x;

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
