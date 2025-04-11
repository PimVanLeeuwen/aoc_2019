#include "Day7.h"


#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <limits.h>
#include <sstream>

int64_t intcode_computer_3(std::vector<int64_t> input, const std::vector<int64_t> &user_input, int64_t previous_amplifier_input) {
    bool user_side = true;
    size_t i = 0;
    size_t iterations = 0;
    int64_t prev_input = previous_amplifier_input;
    size_t input_i = 0;


    while (i < input.size()) {
        std::string operation = std::to_string(input[i]);


        while (operation.size() < 5) operation.insert(0, "0");

        const int opcode = stoi(operation.substr(3));

        if (opcode == 99 && iterations < 4) {
            iterations++;
            i = 0;
            continue;
        } else if (opcode == 99) {
            return prev_input;
        }

        int a, b;

        if (operation[2] == '0') {
            a = input[input[i+1]];
        } else {
            a = input[i+1];
        }

        if (operation[1] == '0') {
            b = input[input[i+2]];
        } else {
            b = input[i+2];
        }

        switch (opcode) {

            case 1:
                assert(operation[0] == '0');
                input[input[i+3]] = a + b;
                i += 4;
                break;
            case 2:
                assert(operation[0] == '0');
                input[input[i+3]] = a * b;
                i += 4;
                break;
            case 3:
                assert(operation[2] == '0');
                if (user_side) {
                    input[input[i+1]] = user_input[input_i++];
                    user_side = false;
                } else {
                    input[input[i+1]] = prev_input;
                    user_side = true;
                }

                i += 2;
                break;
            case 4:
                prev_input = a;
                i += 2;
                break;
            case 5:
                if (a != 0) {
                    i = b;
                } else {
                    i += 3;
                }
                break;
            case 6:
                if (a == 0) {
                    i = b;
                } else {
                    i += 3;
                }
                break;
            case 7:
                input[input[i+3]] = a < b ? 1 : 0;
                i += 4;
                break;
            case 8:
                input[input[i+3]] = a == b ? 1 : 0;
                i += 4;
                break;
            default:
                assert(false);
        }
    }

    return prev_input;
}

void Day7::execute(const std::vector<std::string>& lines) {
    std::vector<int64_t> input;
    std::vector<int64_t> parameters = {0,1,2,3,4};
    std::vector<int64_t> parameters_2 = {5,6,7,8,9};

    std::stringstream ss(lines.front());
    std::string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }

    int64_t part_1 = 0;

    while (std::ranges::next_permutation(parameters).found) {
        part_1 = std::max(part_1, intcode_computer_3(input, parameters, 0));
    }

    std::cout << "Part 1: " << part_1 << std::endl;
}
