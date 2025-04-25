#include "Chip8.h"

Chip8::Chip8() {
    initialize();
};

// Initialize function 
void Chip8::initialize(){
    // Clear
    memory.fill(0); 
    V.fill(0);
    stack.fill(0);
    // TODO => Display  

    // resets 
    pc = 0x200; // pc starts at location 0x200
    I = 0x00; 
    sp = 0x00;
    opcode = 0x00; 
    // TODO => Font 
    // TODO => sound/timers 
}

void Chip8::loadRom(){

}


void Chip8::cycle() {
    // Fetch -> read 2 bytes and combine 
    opcode = (memory[pc] << 8 | memory[pc + 1]);
    instruction_type = opcode & 0xF000 
    
    pc += 2; 

    // decode and exe 
    switch(instruction_type) {
        // Let's say type is 0xA -> Instruction is ANNN
        case 0xA:
            // Set index register to NNN 
            I = opcode & 0x0FFF;
            break;
        default: 
            break; 
    }

    // timers

}; 

// Create a switch case statement to mask off with binary AND the first number. 
// First nibble = type of instructions 
// X  -> second nibble -> looks up V0 through VF register / VX 
// Y  -> third nibble -> looks up V0 through VF / VY 
// N  -> fourth nibble -> 4-bit number 
// NN -> third and fourth 8 bit
// NNN -> second, third and fourth nibble. 12 bit
// Create variables for this -> current_opcode, nibble1, x_index, y_index, n_value

