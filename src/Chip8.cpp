#include "Chip8.h"
#include <iostream>
#include <fstream> 
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
    // TODO => Fontset initialized at memory location 0x50 == 80 and onwards .
    // TODO => sound/timers 
}

// filename is pointer to a read-only / const char 
bool Chip8::loadRom(const char * filename){
    // Initialize 
    initialize();

    //open in binary mode, ate sets the stream's position to the end on opening. 
    
    std::ifstream rom_file(filename, std::ios::binary | std::ios::ate);

    if(!rom_file){
        std::cerr << "Failed to open the file."; 
        return false; 
    }
    //filesize 
    std::streamsize rom_size = file.tellg(); 
    std::cout << "Rom Size is: " << rom_size << std::endl; 
    // seekg + beg, move to x bytes from beginning 
    rom_file.seekg(0, std::ios::beg); // rewind 

    // buffer / mem allocation 
    std::vector<char> buffer(file_size); 

    // reads from file and adds to buffer, data gives access to the internal vector array. We get a pointer to the first element in the vector.
    if(!rom_file.read(buffer.data(), file_size)){
        std::cerr << "Failed to read the file.\n";
        return false; 
    } else{
        std::cout << "File read and added to buffer successfully." << std::endl;
    };

    // Store elements from buffer into chip 8 memory 
    // we loop through the memory array, starting at 512 in memory and while i is less than the size of the buffer
    // check if the file size is not bigger than the available memory size. 
    if(file_size < (4096 - 512)){
        for(int i = 0; i < buffer.size(); ++i){
            memory[512 + i] = buffer[i];
        }
    }

    rom_file.close();
    return true;
    // free(buffer) not needed vector handles this. 
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

