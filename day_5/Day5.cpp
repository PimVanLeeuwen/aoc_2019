#include "Day5.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

int64_t compute_result_2(std::vector<int64_t> input) {
    std:bool halt = false;
    size_t i = 0;


    while (i < input.size() && !halt) {
        std::string operation = std::to_string(input[i]);


        while (operation.size() < 5) operation.insert(0, "0");
        // std::cout << operation << std::endl;

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

        switch (int opcode = stoi(operation.substr(3))) {

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
                std::cout << "Input: ";
                int user_input; std::cin >> user_input;
                assert(operation[2] == '0');
                input[input[i+1]] = user_input;
                i += 2;
                break;
            case 4:
                std::cout << a << std::endl;
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
            case 99:
                halt = true;
                break;
            default:
                std::cout << operation << std::endl;
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

    compute_result_2(input);
}
