
#include <print>
#include <cmath>
#include <ranges>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace r = std::ranges;


template <typename Left, typename Right>
void read_lines(std::ifstream& file, Left lefta, Right righta) {
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ints(line);
        int left, right;
        ints >> left >> right;
        lefta(left);
        righta(right);
    }
}


auto first () -> void {
    std::ifstream file("../inputs/1.txt");
    std::vector<int> lefts;
    std::vector<int> rights;
    read_lines(file,
        [&lefts](int left) { lefts.push_back(left); },
        [&rights](int right) { rights.push_back(right); }
    );

    std::sort(lefts.begin(), lefts.end());
    std::sort(rights.begin(), rights.end());
    auto zp = r::zip_view(lefts, rights);
    int sum = std::accumulate(zp.begin(), zp.end(), 0, [](int acc, auto&& pair) {
        return acc + std::abs(pair.first - pair.second);
    });
    std::println("{}", sum);
}

auto second () -> void {
    std::ifstream file("../inputs/1.txt");
    std::vector<int> lefts;
    std::unordered_map<int, int> rights;
    read_lines(file,
        [&lefts](int left) { lefts.push_back(left); },
        [&rights](int right) { rights[right]++; }
    );

    int total = std::accumulate(lefts.begin(), lefts.end(), 0, [rights](int acc, auto&& left) {
        int matches = 0;
        auto it = rights.find(left);
        if (it != rights.end()) {
            matches = it->second;
        }
        int score = left * matches;
        return acc + score;
    });
    std::println("{}", total);
}


auto main( ) -> int {
    first();
    second();

    return 0;
}
