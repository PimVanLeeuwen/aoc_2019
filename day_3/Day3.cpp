#include "Day3.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>

struct point {
    int64_t x;
    int64_t y;

    [[nodiscard]] int64_t distance_to_zero() const {
        return abs(x) + abs(y);
    }
};

struct lineSegment {
    point a;
    point b;

    bool horizontal = a.y == b.y;

    [[nodiscard]] std::optional<point> intersect(const lineSegment line_2) const {

        if (horizontal == line_2.horizontal) {
            return {};
        }

        if (horizontal && ((line_2.a.y <= a.y && line_2.b.y >= a.y) || (line_2.b.y <= a.y && line_2.a.y >= a.y))
            && ((line_2.a.x <= a.x && line_2.a.x >= b.x) || (line_2.a.x <= b.x && line_2.a.x >= a.x))) {
            return point(line_2.a.x, a.y);
        }

        if (!horizontal && ((a.y <= line_2.a.y && b.y >= line_2.a.y) || (b.y <= line_2.a.y && a.y >= line_2.a.y))
            && ((a.x <= line_2.a.x && a.x >= line_2.b.x) || (a.x <= line_2.b.x && a.x >= line_2.a.x))) {
            return point(a.x, line_2.a.y);
        }

        return {};
    }
};

std::map<char, int> diff_x = {
    {'U', 0},
    {'D', 0},
    {'R', 1},
    {'L', -1},
};

std::map<char, int> diff_y = {
    {'U', 1},
    {'D', -1},
    {'R', 0},
    {'L', 0},
};

void Day3::execute(const std::vector<std::string>& lines) {

    std::vector<std::vector<lineSegment>> wires = {{}, {}};



    for (size_t i = 0; i < lines.size(); i++) {
        std::stringstream ss(lines[i]);
        std::string interim_result;
        point start = {0,0};

        while (getline(ss, interim_result, ',')) {
            char dir = interim_result[0];
            const int64_t amount = stol(interim_result.substr(1));

            point new_point = {start.x + amount*diff_x[dir], start.y + amount*diff_y[dir]};

            wires[i].emplace_back(start, new_point);
            start = new_point;
        }
    }

    int64_t max_dist = INT32_MAX;

    for (lineSegment line_1 : wires[0]) {
        for (lineSegment line_2 : wires[1]) {
            if (std::optional<point> intersect = line_1.intersect(line_2); intersect && intersect.value().distance_to_zero() < max_dist) {
                max_dist = intersect.value().distance_to_zero();
            }
        }
    }

    std::cout << "Part 1: " << max_dist << std::endl;
}