#include "Day8.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <sstream>

using namespace std;

constexpr int WIDTH = 25;
constexpr int HEIGHT = 6;

// image layer object
typedef struct Layer {
    int width;
    int height;
    map<int, int> occ;
    vector<vector<int>> pixels;

    Layer(const int width, const int height) : width(width), height(height), pixels(height, vector(width, 0)) {};

    // print the layer (needed for part 2)
    void print() const {
        for (const auto& row : pixels) {
            for (const auto& pixel : row) {
                if (pixel == 1) {
                    cout << pixel;
                } else {
                    cout << " ";
                }
            }
            cout << endl;
        }
    }
} Layer;

void Day8::execute(const vector<string>& lines) {
    assert(lines[0].size() % (WIDTH*HEIGHT) == 0);
    vector layers(lines[0].size() / (WIDTH*HEIGHT), Layer(WIDTH,HEIGHT));

    int min_0_layer = -1;
    int min_0_count = INT16_MAX;

    // fill the layers
    for (int l = 0; l < layers.size(); l++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                const size_t index = WIDTH*HEIGHT*l + y*WIDTH + x;
                layers[l].pixels[y][x] = lines[0][index] - '0';
                layers[l].occ[lines[0][index] - '0']++;
            }
        }

        // keep track of the layer with the least amount of zeroes
        if (layers[l].occ[0] < min_0_count) {
            min_0_count = layers[l].occ[0];
            min_0_layer = l;
        }
    }

    // part 1
    cout << "Part 1: " << layers[min_0_layer].occ[1]*layers[min_0_layer].occ[2] << endl;

    // layer for the final image
    auto part_2_layer = Layer(WIDTH, HEIGHT);

    // determine every pixel in de final image
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            for (auto & layer : layers) {
                // just skip the transparent ones
                if (layer.pixels[y][x] != 2) {
                    part_2_layer.pixels[y][x] = layer.pixels[y][x];
                    break;
                }
            }
        }
    }

    cout << "Part 2: " << endl;
    part_2_layer.print();

}
