#include "Day7.h"

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
constexpr OpCode END = 99;

// ParameterMode int
typedef int ParameterMode;
constexpr ParameterMode POSITION = 0;
constexpr ParameterMode IMMEDIATE = 1;

// IntCode struct
typedef struct IntCode {
    int modes_n_opcode;     // The full int of the instruction
    OpCode op_code;         // after deconstruction, the op_code
    ParameterMode mode_a;   // after deconstruction, the mode of a
    ParameterMode mode_b;   // after deconstruction, the mode of b
    ParameterMode mode_c;   // after deconstruction, the mode of c
    int a;                  // argument 1
    int b;                  // argument 2
    int c;                  // argument 3
    int offset;             // offset depending on the opcode
} IntCode;

// VM struct
typedef struct VM {
    char tag;                   // identifier
    size_t pc;                  // program counter
    std::vector<int> tape;      // the program to work on
    std::deque<int> inputs;     // the inputs given to the machine
    int output;                 // an output of the machine
    bool halted;                // halted or not
    bool paused;                // paused or not

    // constructor
    VM(const char t, std::vector<int> program, int const settings)
        : tag(t), pc(0), tape(std::move(program)), output(0), halted(false), paused(false) {
        inputs.push_back(settings);
    }
} VM;

// other defined functions
void exec(const std::vector<int>& input, bool part_1);
std::vector<std::vector<int>> phase_generator(int min_range, int max_range);
int amplifier(VM &vm, int input);
void run_program(VM &vm);
IntCode read_instruction(const VM &vm);
void run_instruction(VM &vm, IntCode const &instruction);
int amplify_signal(bool feedback_loop_mode, const std::vector<int>& phase, const std::vector<int>& tape);

// Main function of this file
void Day7::execute(const std::vector<std::string>& lines) {

    // gathering input and putting it into an array
    std::vector<int> input;

    std::stringstream ss(lines.front());
    std::string interim_result;

    while (getline(ss, interim_result, ',')) {
        input.push_back(stoi(interim_result));
    }
    // end gathering input

    // process the two parts
    exec(input, true);
    exec(input, false);
}

void exec(const std::vector<int>& input, const bool part_1) {
    const std::vector<std::vector<int>> sequences = part_1 ? phase_generator(0, 5) :
        phase_generator(5, 9);
    int max_output = 0;

    for (const std::vector<int>& s : sequences) {
        if (int output; (output = amplify_signal(!part_1, s, input)) > max_output) {
            max_output = output;
        }
    }

    if (part_1) {
        std::cout << "Part 1: " << max_output << std::endl;
    } else {
        std::cout << "Part 2: " << max_output << std::endl;
    }
}

// generate all possible 5-digit inputs between min and max (inclusive)
std::vector<std::vector<int>> phase_generator(int min_range, int max_range) {
    // range should be between 0 and 9 inclusive to make sense
    assert(min_range < 10);
    assert(max_range < 10);
    assert(min_range >= 0);
    assert(max_range >= 0);

    std::vector<std::vector<int>> sequences;

    for (int i = min_range; i <= max_range; i++) {
        for (int j = min_range; j <= max_range; j++) {
            for (int k = min_range; k <= max_range; k++) {
                for (int l = min_range; l <= max_range; l++) {
                    for (int m = min_range; m <= max_range; m++) {
                        if (i == j || i == k || i == l || i == m || j == k || j == l || j == m || k == l || k == m || l == m) {
                            continue;
                        }
                        sequences.push_back({i, j, k, l, m});
                    }
                }
            }
        }
    }

    return sequences;
}

int amplify_signal(bool const feedback_loop_mode, std::vector<int> const& phase, std::vector<int> const& tape) {
    // process this phase into output
    VM vm_a('A', tape, phase[0]);
    VM vm_b('B', tape, phase[1]);
    VM vm_c('C', tape, phase[2]);
    VM vm_d('D', tape, phase[3]);
    VM vm_e('E', tape, phase[4]);

    int signal = 0;
    while (!vm_e.halted) {
        signal = amplifier(vm_a, signal);
        signal = amplifier(vm_b, signal);
        signal = amplifier(vm_c, signal);
        signal = amplifier(vm_d, signal);
        signal = amplifier(vm_e, signal);
        if (!feedback_loop_mode) {
            break;
        }
    }
    return signal;

}

int amplifier(VM &vm, int const input) {
    vm.inputs.push_back(input);
    run_program(vm);
    return vm.output;
}

void run_program(VM &vm) {
    // if this does not run anymore
    if (vm.halted) {
        std::cout << "Abort! " << vm.tag << " was already halted";
        return;
    }

    vm.paused = false;

    // whilst we can do operations
    while (vm.pc < vm.tape.size()) {
        // fetch and run the instruction
        IntCode instruction = read_instruction(vm);
        run_instruction(vm, instruction);

        // we stop once there is a halting or pausing occurring
        if (vm.halted || vm.paused) {
            break;
        }

    }
}

// converts 5-digit number to array
std::array<int, 5> to_array(int n) {
    std::array result = {0,0,0,0,0};

    for (int i = 4; i >= 0; i--) {
        const int right = n % 10;
        result[i] = right;
        n /= 10;
    }

    return result;
}

IntCode read_instruction(const VM &vm) {
    IntCode instruction;
    instruction.modes_n_opcode = vm.tape[vm.pc];
    const std::array<int, 5> modes_n_opcode = to_array(vm.tape[vm.pc]);

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
            instruction.a = instruction.mode_a == IMMEDIATE ? vm.tape[vm.pc + 1] : vm.tape[vm.tape[vm.pc + 1]];
            instruction.b = instruction.mode_b == IMMEDIATE ? vm.tape[vm.pc + 2] : vm.tape[vm.tape[vm.pc + 2]];

            assert(instruction.mode_c == POSITION); // placing arguments is always in position mode
            instruction.c = vm.tape[vm.pc + 3];
            break;
            // Input output
        case INPUT:
        case OUTPUT:
            instruction.offset = 2;
            assert(instruction.mode_a == POSITION);
            instruction.a = vm.tape[vm.pc + 1];
            instruction.b = -1; // not used
            instruction.c = -1; // not used
            break;
        case JUMP_FALSE:
        case JUMP_TRUE:
            instruction.offset = 3;
            instruction.a = instruction.mode_a == IMMEDIATE ? vm.tape[vm.pc + 1] : vm.tape[vm.tape[vm.pc + 1]];
            instruction.b = instruction.mode_b == IMMEDIATE ? vm.tape[vm.pc + 2] : vm.tape[vm.tape[vm.pc + 2]];
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

void run_instruction(VM &vm, IntCode const &instruction) {
    // if not a valid opcode
    if (instruction.op_code != ADD && instruction.op_code != MULT &&
        instruction.op_code != INPUT && instruction.op_code != OUTPUT &&
        instruction.op_code != JUMP_FALSE && instruction.op_code != JUMP_TRUE &&
        instruction.op_code != LESS_THAN && instruction.op_code != EQUALS && instruction.op_code != END) {
        std::cout << "Invalid op_code = " << instruction.op_code << std::endl;
        std::cout << vm.tape[vm.pc] << "," << vm.tape[vm.pc + 1] << "," << vm.tape[vm.pc + 2] << "," << vm.tape[vm.pc + 3] << std::endl;

        // program counter to the end and halt
        vm.pc = vm.tape.size();
        vm.halted = true;
        return;
    }

    int result;
    const int a = instruction.a;
    const int b = instruction.b;
    const int c = instruction.c;

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
            if (a == 0) vm.pc = b;
            return;
        case JUMP_TRUE:
            if (a != 0) vm.pc = b;
            return;
        case LESS_THAN:
            vm.tape[c] = a < b ? 1 : 0;
            break;
        case EQUALS:
            vm.tape[c] = a == b ? 1 : 0;
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
