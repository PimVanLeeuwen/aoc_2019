#include "Day10.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <sstream>
#include <cassert>
#include <map>
#include <numbers>
#include <cmath>
#include <set>

using namespace std;

// IntCode struct
typedef struct Asteroid {
    int y;
    int x;
    map<double, set<tuple<double, pair<int, int>>>> angles_extended = {};

    // constructor
    Asteroid(const int y, const int x) : y(y), x(x) {}

    // angle to another asteroid where up is angle 0;
    [[nodiscard]] double angle_to(const Asteroid& b) const {
        assert(b.x != x || b.y != y); // different asteroid
        const double t = atan2((b.x - x),-(b.y - y)) * (180.0 / numbers::pi);
        return t < 0 ? t + 360 : t;
    }

    // distance to another asteroid
    [[nodiscard]] double distance_to(const Asteroid& b) const {
        assert(b.x != x || b.y != y); // different asteroid
        return sqrt(pow(b.x - x, 2) + pow(b.y - y, 2));
    }


} Asteroid;

void Day10::execute(const vector<string>& lines) {

    vector<Asteroid> asteroids;

    // index all asteroids
    for (int y = 0; y < lines.size(); y++) {
        for (int x = 0; x < lines[0].size(); x++) {
            if (lines[y][x] == '#') {
                asteroids.emplace_back(y, x);
            }
        }
    }

    // dummy best asteroid
    Asteroid best_asteroid(0,0);

    for (Asteroid a : asteroids) {
        for (const Asteroid& b : asteroids) {
            if (a.x == b.x && a.y == b.y) {
                continue;
            }

            // keep track of the angles to other asteroids, the distance of those and the coordinates
            // because these are maps and sets, they are sorted per default.
            a.angles_extended[a.angle_to(b)].insert({a.distance_to(b), {b.x,b.y}});
        }

        // keep track of the best asteroid
        if (a.angles_extended.size() > best_asteroid.angles_extended.size()) {
            best_asteroid = a;
        }
    }

    // amount of visible asteroids is the amount of angles listed
    cout << "Part 1: " << best_asteroid.angles_extended.size() << endl;

    // fetch the 200th asteroid
    const auto it = next(best_asteroid.angles_extended.begin(), 199);
    auto [distance, point] = *it->second.begin();

    cout << "Part 2: " << point.first*100 + point.second << endl;
}
