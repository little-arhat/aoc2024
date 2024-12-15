
#include <deque>
#include "utils.hpp"

using pi = std::pair<int, int>;

// y, x
static const std::unordered_map<char, pi> DIRS = {
    {'^', {-1, 0}},
    {'v', {1, 0}},
    {'>', {0, 1}},
    {'<', {0, -1}},
};


void print_grid(const std::vector<std::vector<char>>& grid, bool clear) {
    if (clear) {
        std::print("\033[2J\033[H");
    }

    auto rr = 0;
    std::print(" ");
    for (size_t cc = 0; cc < grid[0].size(); cc++) {
        std::print("{}", cc % 10);
    }
    std::println();

    for (const auto& row : grid) {
        std::print("{}", rr % 10);
        rr++;
        for (const auto& cell : row) {
            if (cell == '@') {
                std::print("\033[38;5;51m@\033[0m");
            } else if (cell == '#') {
                std::print("\033[1m#\033[0m");
            } else if (cell == 'O' || cell == '[' || cell == ']') {
                std::print("\033[38;5;196m{}\033[0m", cell);
            } else {
                std::print("{}", cell);
            }
        }
        std::println();
    }
    std::cout.flush();
}


auto enlarge(const std::string line) -> std::string {
    std::string result;
    result.reserve(line.size());

    for (char c : line) {
        if (c == '#') {
            result += "##";
        } else if (c == 'O') {
            result += "[]";
        } else if (c == '.') {
            result += "..";
        } else if (c == '@') {
            result += "@.";
        } else {
            result += c;
        }
    }
    return result;
}


auto second(std::string s) -> void {
    std::ifstream file(s);

    std::vector<std::vector<char>> grid;
    std::vector<char> moves;
    pi robot = {-1, -1};  // y, x
    bool have_map = false;

    read_lines(file, [&grid, &have_map, &robot, &moves](std::string line) {
        if (line.empty()) {
            have_map = true;
            return;  // cont
        }
        line = enlarge(line);

        if (!have_map) {
            grid.emplace_back(line.begin(), line.end());
            auto p = line.find('@');
            if (p != std::string::npos) {
                robot = {grid.size() - 1, p};
            }
        } else {
            moves.insert(moves.end(), line.begin(), line.end());
        }
    });

    for (auto move : moves) {
        auto [dy, dx] = DIRS.at(move);
        auto [ry, rx] = robot;
        auto nx = rx + dx;
        auto ny = ry + dy;

        // print_grid(grid, false);
        // std::println("Move: {} ({}/{}); r={}. obj={}; press to move",
        //              move,
        //              mc,
        //              moves.size(),
        //              robot,
        //              grid[ny][nx]);
        // std::cin.get();

        if (grid[ny][nx] == '#') {
            /// nothing happens, we just stay
        } else if (grid[ny][nx] == '.') {
            robot = {ny, nx};
            grid[ry][rx] = '.';
            grid[ny][nx] = '@';
        } else if ((grid[ny][nx] == '[' || grid[ny][nx] == ']') &&
                   (move == '<' || move == '>')) {
            // pushing box horizontally
            // scan in the direction of move
            auto nnx = nx + dx;
            auto moving_boxes = 1;
            while (grid[ny][nnx] == '[' || grid[ny][nnx] == ']') {
                nnx += dx;
                moving_boxes += 1;
            }

            // wall
            if (grid[ny][nnx] == '#') {
                // can't move
            } else if (grid[ny][nnx] == '.') {
                // can shift to first gap
                grid[ry][rx] = '.';
                grid[ny][nx] = '@';
                robot = {ny, nx};

                auto bx = nx;
                for (auto b = 1; b <= moving_boxes; b += 2) {
                    bx = bx + dx + dx;
                    auto bx_close = bx - dx;
                    grid[ny][bx] = move == '<' ? '[' : ']';
                    grid[ny][bx_close] = move == '<' ? ']' : '[';
                }
            }
        } else if (grid[ny][nx] == '[' || grid[ny][nx] == ']') {
            // pushing vertically
            auto prev_leftx = grid[ny][nx] == '[' ? nx : nx - 1;
            auto prev_rightx = grid[ny][nx] == '[' ? nx + 1 : nx;
            bool can_move = true;
            bool should_move = true;
            auto nny = ny;
            std::deque<std::tuple<int, int, int>> levels;
            levels.push_front({nny, prev_leftx, prev_rightx});
            while (can_move && should_move) {
                nny = nny + dy;
                int culx = grid[0].size();
                int curx = -1;
                should_move = false;
                for (auto cx = int(prev_leftx); cx <= int(prev_rightx); cx++) {
                    if (grid[nny][cx] == ']') {
                        culx = std::min(culx, cx - 1);
                        curx = std::max(curx, cx);
                        should_move = true;
                    } else if (grid[nny][cx] == '[') {
                        curx = std::max(curx, cx + 1);
                        culx = std::min(culx, cx);
                        should_move = true;
                    } else if (grid[nny][cx] == '#') {
                        can_move = false;
                        break;
                    } else {
                    }
                }
                if (should_move) {
                    levels.push_front({nny, culx, curx});
                }
                prev_rightx = curx;
                prev_leftx = culx;
            }
            if (can_move) {
                for (auto [iy, leftx, rightx] : levels) {
                    for (auto ix = leftx; ix <= rightx; ix++) {
                        if (grid[iy][ix] != '#' && grid[iy + dy][ix] != '#') {
                            std::swap(grid[iy + dy][ix], grid[iy][ix]);
                        }
                    }
                }
                grid[ry][rx] = '.';
                grid[ny][nx] = '@';
                robot = {ny, nx};
            }
        }
    }
    print_grid(grid, false);

    long gps_sum = 0;
    for (size_t row = 0; row < grid.size(); row++) {
        for (size_t col = 0; col < grid[0].size(); col++) {
            if (grid[row][col] == '[') {
                gps_sum += (100 * row + col);
            }
        }
    }
    std::println("{}", gps_sum);
}


auto first(std::string s) -> void {
    std::ifstream file(s);

    std::vector<std::vector<char>> grid;
    std::vector<char> moves;
    pi robot = {-1, -1};  // y, x
    bool have_map = false;

    read_lines(file, [&grid, &have_map, &robot, &moves](std::string line) {
        if (line.empty()) {
            have_map = true;
            return;  // cont
        }

        if (!have_map) {
            grid.emplace_back(line.begin(), line.end());
            auto p = line.find('@');
            if (p != std::string::npos) {
                robot = {grid.size() - 1, p};
            }
        } else {
            moves.insert(moves.end(), line.begin(), line.end());
        }
    });

    for (auto move : moves) {
        auto [dy, dx] = DIRS.at(move);
        auto [ry, rx] = robot;
        auto nx = rx + dx;
        auto ny = ry + dy;

        // print_grid(grid);
        // std::println("Move: {}; r={}. obj={}; press to move",
        //              move,
        //              robot,
        //              grid[ny][nx]);
        // std::cin.get();

        if (grid[ny][nx] == '#') {
            /// nothing happens, we just stay
        } else if (grid[ny][nx] == '.') {
            robot = {ny, nx};
            grid[ry][rx] = '.';
            grid[ny][nx] = '@';
        } else if (grid[ny][nx] == 'O') {
            // scan in the direction of move
            auto nnx = nx + dx;
            auto nny = ny + dy;
            // collect all boxes
            auto moving_boxes = 1;
            while (grid[nny][nnx] == 'O') {
                nnx += dx;
                nny += dy;
                moving_boxes += 1;
            }
            // wall
            if (grid[nny][nnx] == '#') {
                // can't move
            } else if (grid[nny][nnx] == '.') {
                // can shift to first gap
                grid[ry][rx] = '.';
                grid[ny][nx] = '@';
                robot = {ny, nx};
                auto bx = nx;
                auto by = ny;
                for (auto b = 1; b <= moving_boxes; b++) {
                    bx += dx;
                    by += dy;
                    grid[by][bx] = 'O';
                }
            }
        }
    }
    long gps_sum = 0;
    for (size_t row = 0; row < grid.size(); row++) {
        for (size_t col = 0; col < grid[0].size(); col++) {
            if (grid[row][col] == 'O') {
                gps_sum += (100 * row + col);
            }
        }
    }
    print_grid(grid, false);

    std::println("{}", gps_sum);
}

auto main(int argc, char* argv[]) -> int {
    std::string filename = aoc(argc, argv, "../inputs/15.txt");
    first(filename);
    second(filename);
    return 0;
}
