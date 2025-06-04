#include "Day11.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <cassert>
#include <ostream>
#include <sstream>
#include <utility>
#include <set>

using namespace std;

// OpCode int
typedef int OpCode;
constexpr OpCode ADD = 1;
constexpr OpCode MULT = 2;
constexpr OpCode INPUT = 3;
constexpr OpCode OUTPUT = 4;
constexpr OpCode JUMP_TRUE = 5;
constexpr OpCode JUMP_FALSE = 6;
constexpr OpCode LESS_THAN = 7;
constexpr OpCode EQUALS = 8;
constexpr OpCode RELATIVE_ADJUST = 9;
constexpr OpCode END = 99;

// ParameterMode int
typedef int ParameterMode;
constexpr ParameterMode POSITION = 0;
constexpr ParameterMode IMMEDIATE = 1;
constexpr ParameterMode RELATIVE = 2;

typedef int Direction;
constexpr Direction UP = 0;
constexpr Direction RIGHT = 1;
constexpr Direction DOWN = 2;
constexpr Direction LEFT = 3;

// IntCode struct
typedef struct IntCode {
    long modes_n_opcode;     // The full int of the instruction
    OpCode op_code;         // after deconstruction, the op_code
    ParameterMode mode_a;   // after deconstruction, the mode of a
    ParameterMode mode_b;   // after deconstruction, the mode of b
    ParameterMode mode_c;   // after deconstruction, the mode of c
    long a;                  // argument 1
    long b;                  // argument 2
    long c;                  // argument 3
    int offset;             // offset depending on the opcode
} IntCode;

// VM struct
typedef struct VM {
    char tag;                   // identifier
    size_t pc;                  // program counter
    std::vector<long> tape;      // the program to work on
    std::deque<long> inputs;     // the inputs given to the machine
    long output;                 // an output of the machine
    bool halted;                // halted or not
    bool paused;                // paused or not
    long relative_offset;

    // constructor
    VM(const char t, std::vector<long> program)
        : tag(t), pc(0), tape(std::move(program)), output(0), halted(false), paused(false), relative_offset(0) {
        tape.resize(tape.size() + 10000, 0); // add additional zeros for day 9
    }
} VM;

// The painter Robot
typedef struct Robot {
    int x; // coordinates
    int y;
    Direction d; // direction
    VM vm; // IntCode computer

    explicit Robot(VM vm) : x(0), y(0), d(UP), vm(std::move(vm)) {}

    void turn_right() { // turn clockwise
        d = d == LEFT ? UP : d+1;
    }

    void turn_left() { // turn counterclockwise
        d = d == UP ? LEFT : d-1;
    }

    // move according to the current direction one tile forward
    void move_forward() {
        switch (d) {
            case UP:
                y++;
                break;
            case DOWN:
                y--;
                break;
            case LEFT:
                x--;
                break;
            case RIGHT:
                x++;
                break;
            default:
                assert(false);
        }
    }

} Robot;

// other defined functions
pair<set<pair<int, int>>, set<pair<int, int>>> exec_11(const std::vector<long>& input, bool part_2);
void run_program_11(VM &vm);
IntCode read_instruction_11(const VM &vm);
void run_instruction_11(VM &vm, IntCode const &instruction);
void print_paint(const set<pair<int, int>>& tiles);

// Main function of this file
void Day11::execute(const std::vector<std::string>& lines) {

    // gathering input and putting it into an array
    vector<long> input;

    stringstream ss(lines.front());
    string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }
    // end gathering input


    auto [all_tiles_1, white_tiles_1] = exec_11(input, false);
    cout << "Part 1: " << all_tiles_1.size() << endl;

    auto [all_tiles_2, white_tiles_2] = exec_11(input, true);
    print_paint(white_tiles_2);
}

// print a set of coordinates
void print_paint(const set<pair<int, int>>& tiles) {
    int min_x = 0, max_x = 0;
    int min_y = 0, max_y = 0;

    for (auto [x, y] : tiles) {
        min_x = min(x, min_x);
        min_y = min(y, min_y);
        max_x = max(x, max_x);
        max_y = max(y, max_y);
    }

    // print only the white tiles
    for (int y = max_y; y >= min_y; y--) {
        for (int x = min_x; x <= max_x; x++) {
            if (tiles.contains({x, y})) {
                cout << "#";
            } else {
                cout << " ";
            }
        }
        cout << endl;
    }
}


pair<set<pair<int, int>>, set<pair<int, int>>> exec_11(const std::vector<long>& input, bool part_2) {
    set<pair<int, int>> white_tiles = {};
    set<pair<int, int>> visited_tiles = {};

    // part_2 starts on a white tile
    if (part_2) {
        white_tiles.insert({0,0});
    }

    // brains of the robot
    const VM vm_robot('R', input);

    // actual robot
    Robot R(vm_robot);

    // keep going until halted
    while (!R.vm.halted) {
        // provide the input:
        if (white_tiles.contains({R.x, R.y})) {
            // white
            R.vm.inputs.push_back(1);
        } else {
            // black
            R.vm.inputs.push_back(0);
        }

        // Run and get two results back

        run_program_11(R.vm);
        const long to_paint = R.vm.output;
        run_program_11(R.vm);
        const long to_turn = R.vm.output;

        // paint the tile
        visited_tiles.insert({R.x, R.y});
        if (to_paint == 1) {
            // if we paint it white, add to white tiles
            white_tiles.insert({R.x, R.y});
        } else {
            // if we paint it black, try to remove from white tiles
            white_tiles.erase({R.x, R.y});
        }

        // turn & move
        if (to_turn == 1) {
            R.turn_right();
        } else {
            R.turn_left();
        }
        R.move_forward();
    }

    return {visited_tiles, white_tiles};
}

void run_program_11(VM &vm) {
    // if this does not run anymore
    if (vm.halted) {
        return;
    }

    vm.paused = false;

    // whilst we can do operations
    while (vm.pc < vm.tape.size()) {
        // fetch and run the instruction
        IntCode instruction = read_instruction_11(vm);

        run_instruction_11(vm, instruction);

        // we stop once there is a halting or pausing occurring
        if (vm.halted || vm.paused) {
            break;
        }

    }
}

// converts 5-digit number to array
std::array<int, 5> to_array_11(int n) {
    std::array result = {0,0,0,0,0};

    for (int i = 4; i >= 0; i--) {
        const int right = n % 10;
        result[i] = right;
        n /= 10;
    }

    return result;
}

IntCode read_instruction_11(const VM &vm) {
    IntCode instruction;
    instruction.modes_n_opcode = vm.tape[vm.pc];
    const std::array<int, 5> modes_n_opcode = to_array_11(vm.tape[vm.pc]);

    // read the parameter modes
    instruction.mode_a = modes_n_opcode[2];
    instruction.mode_b = modes_n_opcode[1];
    instruction.mode_c = modes_n_opcode[0];

    instruction.op_code = modes_n_opcode[3] * 10 + modes_n_opcode[4];

    switch (instruction.op_code) {
        // these all do the same in terms of arguments
        case ADD:
        case MULT:
        case LESS_THAN:
        case EQUALS:
            instruction.offset = 4;
            // first and second arguments based on instruction mode
            if (instruction.mode_a == IMMEDIATE) {
                instruction.a = vm.tape[vm.pc + 1];
            } else if (instruction.mode_a == RELATIVE) {
                instruction.a = vm.tape[vm.tape[vm.pc + 1] + vm.relative_offset];
            } else {
                assert(instruction.mode_a == POSITION);
                instruction.a = vm.tape[vm.tape[vm.pc + 1]];
            }

            if (instruction.mode_b == IMMEDIATE) {
                instruction.b = vm.tape[vm.pc + 2];
            } else if (instruction.mode_b == RELATIVE) {
                instruction.b = vm.tape[vm.tape[vm.pc + 2] + vm.relative_offset];
            } else {
                assert(instruction.mode_b == POSITION);
                instruction.b = vm.tape[vm.tape[vm.pc + 2]];
            }

            if (instruction.mode_c == RELATIVE) {
                instruction.c = vm.tape[vm.pc + 3] + vm.relative_offset;
            } else {
                assert(instruction.mode_c == POSITION); // cannot be immediate mode
                instruction.c = vm.tape[vm.pc + 3];
            }

            break;
            // Input output
        case INPUT:
            instruction.offset = 2;

            if (instruction.mode_a == RELATIVE) {
                instruction.a = vm.tape[vm.pc + 1] + vm.relative_offset;
            } else {
                assert(instruction.mode_a == POSITION); // cannot be immediate mode
                instruction.a = vm.tape[vm.pc + 1];
            }

            instruction.b = -1; // not used
            instruction.c = -1; // not used
            break;
        case OUTPUT:
            instruction.offset = 2;

            if (instruction.mode_a == IMMEDIATE) {
                instruction.a = vm.tape[vm.pc + 1];
            } else if (instruction.mode_a == RELATIVE) {
                instruction.a = vm.tape[vm.tape[vm.pc + 1] + vm.relative_offset];
            } else {
                assert(instruction.mode_a == POSITION);
                instruction.a = vm.tape[vm.tape[vm.pc + 1]];
            }

            instruction.b = -1; // not used
            instruction.c = -1; // not used
            break;
        case RELATIVE_ADJUST:
            instruction.offset = 2;
            if (instruction.mode_a == IMMEDIATE) {
                instruction.a = vm.tape[vm.pc + 1];
            } else if (instruction.mode_a == RELATIVE) {
                instruction.a = vm.tape[vm.tape[vm.pc + 1] + vm.relative_offset];
            } else {
                assert(instruction.mode_a == POSITION);
                instruction.a = vm.tape[vm.tape[vm.pc + 1]];
            }

            instruction.b = -1; // not used
            instruction.c = -1; // not used
            break;
        case JUMP_FALSE:
        case JUMP_TRUE:
            instruction.offset = 3;
            if (instruction.mode_a == IMMEDIATE) {
                instruction.a = vm.tape[vm.pc + 1];
            } else if (instruction.mode_a == RELATIVE) {
                instruction.a = vm.tape[vm.tape[vm.pc + 1] + vm.relative_offset];
            } else {
                assert(instruction.mode_a == POSITION);
                instruction.a = vm.tape[vm.tape[vm.pc + 1]];
            }

            if (instruction.mode_b == IMMEDIATE) {
                instruction.b = vm.tape[vm.pc + 2];
            } else if (instruction.mode_b == RELATIVE) {
                instruction.b = vm.tape[vm.tape[vm.pc + 2] + vm.relative_offset];
            } else {
                assert(instruction.mode_b == POSITION);
                instruction.b = vm.tape[vm.tape[vm.pc + 2]];
            }

            instruction.c = -1; // not used
            break;
        case END:
            instruction.offset = 1;
            instruction.a = -1; // not used
            instruction.b = -1; // not used
            instruction.c = -1; // not used
            break;
        default:
            assert(false);
    }

    return instruction;
}

void run_instruction_11(VM &vm, IntCode const &instruction) {
    // if not a valid opcode
    if (instruction.op_code != ADD && instruction.op_code != MULT &&
        instruction.op_code != INPUT && instruction.op_code != OUTPUT &&
        instruction.op_code != JUMP_FALSE && instruction.op_code != JUMP_TRUE &&
        instruction.op_code != LESS_THAN && instruction.op_code != EQUALS && instruction.op_code != END &&
        instruction.op_code != RELATIVE_ADJUST) {
        std::cout << "Invalid op_code = " << instruction.op_code << std::endl;
        std::cout << vm.tape[vm.pc] << "," << vm.tape[vm.pc + 1] << "," << vm.tape[vm.pc + 2] << "," << vm.tape[vm.pc + 3] << std::endl;

        // program counter to the end and halt
        vm.pc = vm.tape.size();
        vm.halted = true;
        return;
    }

    long result;
    const long a = instruction.a;
    const long b = instruction.b;
    const long c = instruction.c;

    // do the operation
    switch (instruction.op_code) {
        case ADD:
            // add
            result = a + b;
            vm.tape[c] = result;
            break;
        case MULT:
            // multiply
            result = a * b;
            vm.tape[c] = result;
            break;
        case INPUT:
            // take input, if there is any
            assert(!vm.inputs.empty());
            vm.tape[a] = vm.inputs.front();
            vm.inputs.pop_front();
            break;
        case OUTPUT:
            // output pauses so that it can be retrieved
            vm.output = a;
            vm.paused = true;
            break;
        case JUMP_FALSE:
            if (a == 0) {
                vm.pc = b;
                return;
            }
            break;
        case JUMP_TRUE:
            if (a != 0) {
                vm.pc = b;
                return;
            }
            break;
        case LESS_THAN:
            vm.tape[c] = a < b ? 1 : 0;
            break;
        case EQUALS:
            vm.tape[c] = a == b ? 1 : 0;
            break;
        case RELATIVE_ADJUST:
            vm.relative_offset += a;
            break;
        case END:
            vm.halted = true;
            vm.pc = vm.tape.size();
            return;
        default:
            // should not be anything else
            assert(false);
    }

    vm.pc += instruction.offset;


}
