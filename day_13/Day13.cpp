#include "Day13.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <cassert>
#include <sstream>
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

typedef int TileID;
constexpr TileID EMPTY = 0;
constexpr TileID WALL = 1;
constexpr TileID BLOCK = 2;
constexpr TileID PADDLE = 3;
constexpr TileID BALL = 4;

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

// tiles in the arcade
typedef struct Tile {
    int x;
    int y;
    TileID id;
} Tile;

// The Arcade
typedef struct Arcade {
    vector<vector<TileID>> tiles;
    long score;

    VM vm; // IntCode computer

    explicit Arcade(VM vm) : score(0), vm(std::move(vm)) {
        tiles.resize(23, vector<TileID>(43, 0));
    }

    [[nodiscard]] int part_1() const {
        int result = 0;

        for (size_t y = 0; y < 23; y++) {
            for (size_t x = 0; x < 43; x++) {
                result = tiles[y][x] == BLOCK ? result + 1 : result;
            }
        }

        return result;
    }

    void draw_screen() const {
        cout << "Score: " << score << endl;
        for (size_t y = 0; y < 23; y++) {
            for (size_t x = 0; x < 43; x++) {
                switch (tiles[y][x]) {
                    case EMPTY:
                        cout << ' ';
                        break;
                    case WALL:
                        cout << '#';
                        break;
                    case BLOCK:
                        cout << 'B';
                        break;
                    case PADDLE:
                        cout << 'P';
                        break;
                    case BALL:
                        cout << 'O';
                        break;
                    default:
                        assert(false);
                }
            }
            cout << endl;
        }
    }

} Arcade;

// other defined functions
Arcade exec_13(const std::vector<long>& input);
void run_program_13(VM &vm);
IntCode read_instruction_13(const VM &vm);
void run_instruction_13(VM &vm, IntCode const &instruction);

// Main function of this file
void Day13::execute(const std::vector<std::string>& lines) {
    // gathering input and putting it into an array
    vector<long> input;

    stringstream ss(lines.front());
    string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }
    // end gathering input



    auto A = exec_13(input);
    cout << "Part 1: " << A.part_1() << endl;

    A.vm.tape[0] = 2;
    A.vm.halted = false;

    // 42-22
    A.draw_screen();

    while (true) {
        // collect input
        int j;
        cin >> j;
        A.vm.inputs.push_back(j);

        while (!A.vm.halted) {
            // Run and get three results back

            run_program_13(A.vm);
            const long x = A.vm.output;
            run_program_13(A.vm);
            const long y = A.vm.output;
            run_program_13(A.vm);
            const long id = A.vm.output;

            // process output
            if (x == -1) {
                A.score = id;
            } else {
                A.tiles[y][x] = static_cast<TileID>(id);
            }

        }

        A.draw_screen();
    }
}


Arcade exec_13(const std::vector<long>& input) {
    // brains of the arcade
    const VM vm_arcade('A', input);

    // actual robot
    Arcade A(vm_arcade);

    // keep going until halted
    while (!A.vm.halted) {
        // Run and get three results back

        run_program_13(A.vm);
        const long x = A.vm.output;
        run_program_13(A.vm);
        const long y = A.vm.output;
        run_program_13(A.vm);
        const long id = A.vm.output;

       A.tiles[y][x] = static_cast<TileID>(id);
    }

    return A;
}

void run_program_13(VM &vm) {
    // if this does not run anymore
    if (vm.halted) {
        return;
    }

    vm.paused = false;

    // whilst we can do operations
    while (vm.pc < vm.tape.size()) {
        // fetch and run the instruction
        IntCode instruction = read_instruction_13(vm);


        run_instruction_13(vm, instruction);

        // we stop once there is a halting or pausing occurring
        if (vm.halted || vm.paused) {
            break;
        }

    }
}

// converts 5-digit number to array
std::array<int, 5> to_array_13(int n) {
    std::array result = {0,0,0,0,0};

    for (int i = 4; i >= 0; i--) {
        const int right = n % 10;
        result[i] = right;
        n /= 10;
    }

    return result;
}

IntCode read_instruction_13(const VM &vm) {
    IntCode instruction;
    instruction.modes_n_opcode = vm.tape[vm.pc];
    const std::array<int, 5> modes_n_opcode = to_array_13(vm.tape[vm.pc]);

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

void run_instruction_13(VM &vm, IntCode const &instruction) {
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
