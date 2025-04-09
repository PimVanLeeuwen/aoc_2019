#include "Day4.h"

#include <iostream>
#include <sstream>

int64_t to_digit(const std::vector<int>& pw) {
    return pw[0]*100000 + pw[1]*10000 + pw[2]*1000 + pw[3] * 100 + pw[4] * 10 + pw[5];
}

bool is_non_decreasing(const std::vector<int>& pw) {
    for (size_t i = 0; i < pw.size() - 1; i++) {
        if (pw[i] > pw[i+1]) return false;
    }

    return true;
}

bool valid_password(const std::vector<int> pw) {

    if (to_digit(pw) > 820401) return false;

    for (size_t i = 0; i < pw.size() - 1; i++) {
        if (pw[i] == pw[i+1]) return true;
    }

    return false;
}

bool valid_password_2(const std::vector<int> pw) {

    size_t i = 0;

    while (i < pw.size() - 1) {
        if (pw[i] == pw[i+1]) {
            if (i == pw.size()-2 || pw[i] != pw[i+2]) {
                return true;
            }

            const int digit = pw[i];
            while (pw[i] == digit && i < pw.size()-1) i++;
        } else {
            i++;
        }
    }

    return false;
}

void increment_vector(std::vector<int>& pw) {

    bool done_once = false;

    while (!is_non_decreasing(pw) || !done_once) {
        bool carry = true;

        for (int64_t i = pw.size() - 1; i >= 0; --i) {
            if (carry) {
                pw[i] += 1;
                if (pw[i] > 9) {
                    pw[i] = 0;
                    carry = true;
                } else {
                    carry = false;
                }
            }
        }
        done_once = true;
    }
}

void Day4::execute(const std::vector<std::string>& lines) {

    std::vector init_pw = {3,5,9,2,8,2};
    int64_t part_1 = 0, part_2 = 0;

    while (to_digit(init_pw) <= 820401) {
        if (valid_password(init_pw)) {
            part_1++;
            if (valid_password_2(init_pw)) {
                part_2++;
            }
        }

        increment_vector(init_pw);
    }

    std::cout << "Part 1: " << part_1 << std::endl << "Part 2: " << part_2 << std::endl;
}
