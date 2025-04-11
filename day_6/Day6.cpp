#include "Day6.h"

#include <iostream>
#include <map>
#include <set>
#include <sstream>

void Day6::execute(const std::vector<std::string>& lines) {

    std::map<std::string, std::set<std::string>> orbits;
    std::map<std::string, std::set<std::string>> orbits_2_sided;

    for (const std::string& line : lines) {
        std::string first = line.substr(0, line.find(')'));
        std::string second = line.substr(line.find(')') + 1);

        orbits[first].insert(second);
        orbits_2_sided[first].insert(second);
        orbits_2_sided[second].insert(first);
    }

    uint64_t part_1 = 0, part_2 = 0;

    std::deque<std::pair<int, std::string>> q = {{0, "COM"}};

    while (!q.empty()) {
        auto [connections, planet] = q.front(); q.pop_front();

        part_1 += orbits[planet].size() + orbits[planet].size()*connections;

        for (const std::string& receiver : orbits[planet]) {
            q.emplace_back(connections+1, receiver);
        }
    }

    q = {{0, "YOU"}};
    std::set<std::string> visited;

    while (!q.empty()) {
        auto [connections, planet] = q.front(); q.pop_front();

        visited.insert(planet);

        if (planet == "SAN") {
            part_2 = connections;
            break;
        }

        for (const std::string& receiver : orbits_2_sided[planet]) {
            if (!visited.contains(receiver)) {
                q.emplace_back(connections+1, receiver);
            }
        }
    }

    std::cout << "Part 1: " << part_1 << std::endl;
    std::cout << "Part 2: " << part_2 - 2 << std::endl;
}
