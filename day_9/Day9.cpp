#include "Day9.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <sstream>
#include <utility>

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
    VM(const char t, std::vector<long> program, long const settings)
        : tag(t), pc(0), tape(std::move(program)), output(0), halted(false), paused(false), relative_offset(0) {
        tape.resize(tape.size() + 10000, 0); // add additional zeros for day 9
        inputs.push_back(settings);
    }
} VM;

// other defined functions
long exec_9(const std::vector<long>& input, int start);
void run_program_9(VM &vm);
IntCode read_instruction_9(const VM &vm);
void run_instruction_9(VM &vm, IntCode const &instruction);

// Main function of this file
void Day9::execute(const std::vector<std::string>& lines) {

    // gathering input and putting it into an array
    std::vector<long> input;

    std::stringstream ss(lines.front());
    std::string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stol(interim_result));
    }
    // end gathering input

    // process the two parts
    std::cout << "Part 1: " << exec_9(input, 1) << std::endl;
    std::cout << "Part 2: " << exec_9(input, 2) << std::endl;
}

long exec_9(const std::vector<long>& input, const int start) {

    VM vm_test('T', input, start);
    run_program_9(vm_test);
    return vm_test.output;
}

void run_program_9(VM &vm) {
    // if this does not run anymore
    if (vm.halted) {
        std::cout << "Abort! " << vm.tag << " was already halted";
        return;
    }

    vm.paused = false;

    // whilst we can do operations
    while (vm.pc < vm.tape.size()) {
        // fetch and run the instruction
        IntCode instruction = read_instruction_9(vm);
        // std::cout << "Running at pc: " << vm.pc << std::endl;
        run_instruction_9(vm, instruction);

        // we stop once there is a halting or pausing occurring
        if (vm.halted || vm.paused) {
            break;
        }

    }
}

// converts 5-digit number to array
std::array<int, 5> to_array_9(int n) {
    std::array result = {0,0,0,0,0};

    for (int i = 4; i >= 0; i--) {
        const int right = n % 10;
        result[i] = right;
        n /= 10;
    }

    return result;
}

IntCode read_instruction_9(const VM &vm) {
    IntCode instruction;
    instruction.modes_n_opcode = vm.tape[vm.pc];
    const std::array<int, 5> modes_n_opcode = to_array_9(vm.tape[vm.pc]);

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
        case OUTPUT:
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

    // std::cout << "Offset: " << instruction.offset << std::endl;
    // std::cout << "a: " << instruction.a << std::endl;
    // std::cout << "b: " << instruction.b << std::endl;
    // std::cout << "c: " << instruction.c << std::endl << std::endl;
    return instruction;
}

void run_instruction_9(VM &vm, IntCode const &instruction) {
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
            vm.output = vm.tape[a];
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
