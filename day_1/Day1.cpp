#include "Day1.h"

#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

struct module {
    int64_t mass;

    [[nodiscard]] int64_t get_fuel() const {
        return mass / 3 - 2;
    }

    [[nodiscard]] int64_t get_fuel_part_2() const {
        std::vector added_fuel = { mass / 3 - 2};

        while ( added_fuel.back() / 3 - 2 > 0) {
            added_fuel.push_back(added_fuel.back() / 3 - 2);
        }

        return std::accumulate(added_fuel.begin(), added_fuel.end(), 0);
    }
};

void Day1::execute(const std::vector<std::string>& lines) {

    int64_t part_1 = 0;
    int64_t part_2 = 0;

    for (const std::string& line : lines) {
        auto m = module(stol(line));
        part_1 += m.get_fuel();
        part_2 += m.get_fuel_part_2();
    }

    std::cout << "Part 1: " << part_1 << std::endl;
    std::cout << "Part 2: " << part_2 << std::endl;
}

