#include "Day12.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <numeric>
#include <cassert>
#include <set>
#include <sstream>

using namespace std;

struct Moon {
    int x, y, z;
    int vx, vy, vz;

    Moon(const int x, const int y, const int z)
            : x(x), y(y), z(z), vx(0), vy(0), vz(0) {}

    // update current velocity,
    // positive/negative (at most 1) to compensate for diff
    // if there is no difference, does not change
    void apply_gravity(const Moon m2) {
        vx += max(-1, min(m2.x - x, 1));
        vy += max(-1, min(m2.y - y, 1));
        vz += max(-1, min(m2.z - z, 1));
    }

    // apply the current velocity
    void apply_velocity() {
        x += vx;
        y += vy;
        z += vz;
    }

    // return the potential energy of this moon
    [[nodiscard]] int get_potential_energy() const {
        return abs(x) + abs(y) + abs(z);
    }

    // return the kinetic energy of this moon
    [[nodiscard]] int get_kinetic_energy() const {
        return abs(vx) + abs(vy) + abs(vz);
    }
};

struct Galaxy {
    vector<Moon> moons;

    void add_moon(const int x, const int y, const int z) {
        moons.emplace_back(x, y, z);
    }

    // apply the gravity step between all pairs of moons
    void apply_gravity_all() {
        // note that we do not skip duplicates, but since all coordinates
        // are then the same, velocity does not change.
        for (Moon& m1 : moons) {
            for (const Moon& m2 : moons) {
                m1.apply_gravity(m2);
            }
        }
    }

    // apply the velocity to all moons
    void apply_velocity_all() {
        for (Moon& m : moons) {
            m.apply_velocity();
        }
    }

    // do one step for the entire galaxy
    void do_step() {
        apply_gravity_all();
        apply_velocity_all();
    }

    // print for debugging purposes
    void print_galaxy() const {
        for (const Moon m : moons) {
            cout << "pos=<x=" << m.x << ", y=" <<
                m.y << ", z=" << m.z << ">, vel=<x=" <<
                m.vx << ", y=" << m.vy << ", z=" << m.vz << ">" << endl;
        }
        cout << endl;
    }

    // get the total energy from this system
    [[nodiscard]] int get_total_energy() const {
        int result = 0;

        for (Moon m : moons) {
            result += m.get_kinetic_energy()*m.get_potential_energy();
        }

        return result;
    }

    // return a hash of the d-dimension of this galaxy (since independence)
    [[nodiscard]] vector<int> get_signature(const int d) const {
        vector<int> result;
        switch (d) {
            case 0:
                for (const auto & m : moons) {
                    result.emplace_back(m.x);
                    result.emplace_back(m.vx);
                }
                break;
            case 1:
                for (const auto & m : moons) {
                    result.emplace_back(m.y);
                    result.emplace_back(m.vy);
                }
                break;
            case 2:
                for (const auto & m : moons) {
                    result.emplace_back(m.z);
                    result.emplace_back(m.vz);
                }
                break;
            default:
                assert(false);
        }

        return result;
    }

};

void Day12::execute(const vector<string>& lines) {

    // manually added the moons because of small input
    auto galaxy = Galaxy();
    galaxy.add_moon(-16, 15, -9);
    galaxy.add_moon(-14, 5, 4);
    galaxy.add_moon(2, 0, 6);
    galaxy.add_moon(-3, 18, 9);

    // part 2 repetition finder
    vector<set<vector<int>>> axis_signatures = {{}, {}, {}};
    vector moon_signatures_steps = {0,0,0};

    int steps = 0;
    while (true) {
        galaxy.do_step();
        steps++;

        if (steps == 1000) {
            cout << "Part 1: " << galaxy.get_total_energy() << endl;
        }

        // loop until repetition
        for (int i = 0; i < 3; i++) {
            // do not do dimensions we already have
            if (moon_signatures_steps[i] > 0) {
                continue;
            }

            // check if we repeat
            if (axis_signatures[i].contains(galaxy.get_signature(i))) {
                moon_signatures_steps[i] = steps;
            } else {
                axis_signatures[i].insert(galaxy.get_signature(i));
            }
        }

        // found steps until repetition
        if (moon_signatures_steps[0] > 0 && moon_signatures_steps[1] > 0 &&
            moon_signatures_steps[2] > 0) {
            break;
        }
    }

    cout << "Part 2: " << lcm(lcm(static_cast<long>(moon_signatures_steps[1]),
        static_cast<long>(moon_signatures_steps[0])),
        static_cast<long>(moon_signatures_steps[2])) << endl;

}