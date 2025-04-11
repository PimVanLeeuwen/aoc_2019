#include "Day5.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

int64_t intcode_computer_2(std::vector<int64_t> input, const std::vector<int> &user_input) {
    std:bool halt = false;
    size_t i = 0;
    size_t input_i = 0;


    while (i < input.size() && !halt) {
        std::string operation = std::to_string(input[i]);


        while (operation.size() < 5) operation.insert(0, "0");

        int opcode = stoi(operation.substr(3));

        if (opcode == 99) {
            return input[0];
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
                input[input[i+1]] = user_input[input_i++];
                i += 2;
                break;
            case 4:
                std::cout << "Output: " << a << std::endl;
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

    return input[0];
}

void Day5::execute(const std::vector<std::string>& lines) {
    std::vector<int64_t> input;

    std::stringstream ss(lines.front());
    std::string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }

    intcode_computer_2(input, {1});
    intcode_computer_2(input, {5});
}
