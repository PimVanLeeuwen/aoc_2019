#include "Day14.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <regex>
#include <cassert>
#include <set>
#include <sstream>
#include <valarray>

using namespace std;

void Day14::execute(const vector<string>& lines) {

    vector<pair<pair<int, string>, vector<pair<int, string>>>> reactions;

    for (const string& line : lines) {
        //for every line

        // take the right part (originally left size)
        string right = line.substr(0, line.find("=>"));
        // and the left side (always size 1)
        string left = line.substr(line.find("=>") + 3);

        // one left side item, strip of spaces and split on space
        left = std::regex_replace(left, std::regex("^ +| +$|( ) +"), "$1");
        pair left_processed = {stoi(left.substr(0, left.find(' '))), left.substr(left.find(' ') + 1) };


        // right side may consist of more items
        stringstream ssr(right);
        string interim_result;
        vector<pair<int, string>> right_processed;

        // process the right side
        while (getline(ssr, interim_result, ',')) {
            // remove leading and trailing spaces'
            interim_result = std::regex_replace(interim_result, std::regex("^ +| +$|( ) +"), "$1");

            int amount = stoi(interim_result.substr(0, interim_result.find(' ')));
            string type = interim_result.substr(interim_result.find(' ') + 1);
            right_processed.emplace_back(amount, type);
        }

        reactions.emplace_back(left_processed, right_processed);
    }

    //
    // DONE INPUT PROCESSING
    //

    deque<pair<int, string>> needed = {{1, "FUEL"}};

    int part_1 = 0;

    while (!needed.empty()) {
        auto [mnt, type] = needed.front(); needed.pop_front();
        cout << "TAKING: " << mnt << " * " << type << endl;

        // ore we just count, that is what we need
        if (type == "ORE") {
            cout << "FILLING: " << mnt << endl;
            part_1 += mnt;
            continue;
        }

        for (const auto&[a, b] : reactions) {
            if (a.second != type) {
                continue;
            }

            int complete_times = mnt / a.first;
            int left_times = mnt % a.first;

            if (complete_times > 0) {
                for (const auto&[b_instance_amount, b_instance_type] : b) {
                    cout << "NEEDING: " << b_instance_amount * complete_times << " * " << b_instance_type << endl;
                    needed.emplace_back(b_instance_amount * complete_times, b_instance_type);
                }
            }

            if (left_times > 0) {
                needed.emplace_back(left_times, type);
            }

        }
    }



    cout << "Part 1: " << part_1 << endl;
}
