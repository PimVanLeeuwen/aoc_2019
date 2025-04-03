#include "Day2.h"

#include <iostream>
#include <sstream>
#include <vector>

int64_t compute_result(std::vector<int64_t> input) {
    std:bool halt = false;

    for (size_t i = 0; i < input.size(); i+=4) {
        if (halt) break;
        switch (input[i]) {
            case 1:
                input[input[i+3]] = input[input[i+1]] + input[input[i+2]];
            break;
            case 2:
                input[input[i+3]] = input[input[i+1]] * input[input[i+2]];
            break;
            case 99:
                halt = true;
            break;
            default:
                break;
        }
    }

    return input[0];
}

void Day2::execute(const std::vector<std::string>& lines) {
    std::vector<int64_t> input;

    std::stringstream ss(lines.front());
    std::string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }

    input[1] = 12; input[2] = 2;

    const int64_t part_1 = compute_result(input); int64_t part_2 = -1;

    for (int64_t noun = 0; noun < 100; noun++) {
        for (int64_t verb = 0; verb < 100; verb++) {
            input[1] = noun;
            input[2] = verb;
            if (compute_result(input) == 19690720) {
                part_2 = 100 * noun + verb;
                break;
            }
        }
    }


    std::cout << "Part 1: " << part_1 << std::endl;
    std::cout << "Part 2: " << part_2 << std::endl;
}
