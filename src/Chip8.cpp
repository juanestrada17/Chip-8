#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <cstdint> 
#include <chrono>  
#include <random> 

// Fonts 16 Characters of 5 bytes each = 80 bytes total array . 
// Font is loaded into memory at 0x50 
const unsigned int FONT_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80; 

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    initialize();

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U); 
};

// Initialize function 
void Chip8::initialize(){
    // Clear
    memory.fill(0); 
    V.fill(0);
    stack.fill(0);
    gfx.fill(0);

    // resets 
    pc = 0x200; // pc starts at location 0x200
    I = 0x00; 
    sp = 0x00;
    opcode = 0x00; 
    
    
    uint8_t fontset[FONTSET_SIZE] = 
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONT_START_ADDRESS + 1] = fontset[i];
    }

    delay_timer = 0; 
    sound_timer = 0; 
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
    std::streamsize rom_size = rom_file.tellg(); 
    std::cout << "Rom Size is: " << rom_size << std::endl; 
    // seekg + beg, move to x bytes from beginning 
    rom_file.seekg(0, std::ios::beg); // rewind 

    // buffer / mem allocation 
    std::vector<char> buffer(rom_size); 

    // reads from file and adds to buffer, data gives access to the internal vector array. We get a pointer to the first element in the vector.
    if(!rom_file.read(buffer.data(), rom_size)){
        std::cerr << "Failed to read the file.\n";
        return false; 
    } else{
        std::cout << "File read and added to buffer successfully." << std::endl;
    };

    // Store elements from buffer into chip 8 memory 
    // we loop through the memory array, starting at 512 in memory and while i is less than the size of the buffer
    // check if the file size is not bigger than the available memory size. 
    if(rom_size < (4096 - 512)){
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
    uint16_t instruction_type = opcode & 0xF000;
    uint8_t regX = (opcode & 0x0F00) >> 8; 
    uint8_t regY = (opcode & 0x00F0) >> 4;
    uint8_t byte = (opcode & 0x00FF); 
    uint8_t lastBit = opcode & 0x000F;     

    pc += 2; 

    // decode and exe 
    switch(instruction_type) {
        case 0x0000: 
            if(opcode == 0x00E0){ // Clears screen. 
                for(int i = 0; i < gfx.size(); ++i){
                    gfx[i] = 0; 
                }
            } else if(opcode == 0x00EE) {// Returns a subroutine.  
                --sp; // Goes back stack and pops 
                pc = stack[sp]; // returns to prev saved point. 
            }
            break; 
        case 0x1000: // Jump to NNN 
            pc = opcode & 0x0FFF; 
            break;
        case 0x2000: // call a subroutine (set pc to NNN, store in the stack in order to return)
            stack[sp] = pc; // we store current pc into the stack 
            ++sp; //increment the stack pointer 
            pc = opcode & 0x0FFF;// set pc to NNN
            break;
        case 0x3000: // 3XNN  - Skips to next instruction of VX == NN 
            if(V[regX] == byte){
                pc += 2;
            }
            break; 
        case 0x4000: // 4XNN - skips to next instruction if VX != NN
            if(V[regX] != byte){
                pc += 2; 
            }
            break; 
        case 0x5000: // 5XY0 -> if reg x and reg y are equal skip to next instruction 
            if(V[regX] == V[regY]){
                pc += 2; 
            }
            break; 
        case 0x6000: // 6XNN -> sets value of register X to NN - LD Vx 
            V[regX] = byte; 
            break;
        case 0x7000: // 7xkk -> Add value of regX to byte. 
            V[regX] += byte;
            break; 
        case 0x8000: // 8xy0 load vx to vy
            switch (lastBit){
                case 0: 
                    V[regX] = V[regY]; // Sets x to y 
                    break;
                case 1: 
                    V[regX] = V[regX] | V[regY]; // can also do V[regX] |= V[regY] OR
                    break;
                case 2: 
                    V[regX] = V[regX] & V[regY]; // AND
                    break;
                case 3:
                    V[regX] = V[regX] ^ V[regY]; // XOR 
                    break; 
                case 4: {
                    uint16_t sum = V[regX] + V[regY]; // Add two register with carry flag. 
                    if(sum > 255){ 
                        V[0xF] = 1; 
                    } else {
                        V[0xF] = 0;
                    }
                    uint8_t value = sum & 0xFF; 
                    V[regX] = value; 
                    break;
                }
                case 5:
                    if(V[regX] > V[regY]){
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0; 
                    }
                    V[regX] -= V[regY]; 
                    break;
                case 6: // Vx = Vx SHR 1
                    // SHR 1 shift bit to the right. 
                    // least significant bit is 1 then VF 1.
                    lastBit = V[regX] & 0x000F; 
                    V[0xF] = (V[regX] & 0x1); 
                    V[regX] >>= 1; //shift to right
                    break; 
                case 7: // SubN vx, vy 
                    if(V[regY] > V[regX]){
                        V[0xF] = 1; 
                    } else {
                        V[0xF] = 0; 
                    }
                    V[regX] = V[regY] - V[regX];
                    break; 
                case 0xE: // vx = vx shl 1 
                    V[0xF] = (V[regX] & 0x80) >> 7; // most significate bit. 
                    V[regX] <<= 1; 
                    break;  
                default:
                    std::cout << "0x8000 code not invalid or not recognized " << std::endl; 
            }
            break; 
        case 0x9000: // 9XY0 skip next instruction if Vx != Vy 
            if(V[regX] != V[regY]){
                pc += 2;
            }
            break;  
        case 0xA000:  // Set index register to NNN - ANNN
            I = opcode & 0x0FFF;
            break;
        case 0xB000: // jump to NNN + V0 
            pc = (opcode & 0x0FFF) + V[0]; 
            break; 
        case 0xC000: // perform random AND operation with a random byte and NN  CXNN 
            V[regX] = randByte(randGen) & byte; 
            break; 
        case 0xD000: { // Draw -> Display a n-byte sprite at memory location I at Vx, Vy, set VF = collision 
            // DXYN  
            // Sprite is 8 pixels wide. 
            // if there's a sprite, there's collision. Set VF 
            // XOR screen pixel with 0xFFFFFFFF to flip on or off 
            
            // Get X and Y coordinates from Vx /  Vy 
            uint8_t height = opcode & 0x000F; // height 
            uint8_t width = 8; 
    
            // modulo 64 / 31  
            // wrapping -> cordX % 64 -> 68 % 64 = 4 
            // codY % 32 -> 33 % 32 = 1 
            uint8_t xCord = V[regX] & 63;
            uint8_t yCord = V[regY] & 31; 
            V[0xF] = 0; 
            
            // let's say it's Dx0003 -> height would be 3 so it executes 3 rows  
            for(unsigned int row; row < height; ++row){
                // Starts from index register. 
                uint8_t sprite_byte = memory[I + row]; 
                // loop through each sprite_byte of 8 bits of width 
                for(unsigned int col; col < width; ++col){
                    // bit mask leftmost bit and shifts 0,1,2,3...7 position. 0x80
                    uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
                    // Checks point in screen where it will be drawn
                    uint32_t* screen_pixel = &gfx[(yCord + row) * VIDEO_WIDTH + (xCord + col)];
                    
                    // if sprite_pixel is 1
                    if (sprite_pixel){
                        // we check if the screen already has a 1 at that position 
                        if(*screen_pixel == 0xFFFFFFFF){
                            // if it does we set VF to it 
                            V[0xF] = 1; 
                        }
                    }
                    // Perform XOR over the screen pixel to turn it ON 
                    *screen_pixel ^= 0xFFFFFFFF;                  
                }
            }
            break;
        } 
        case 0xE000:
            switch(byte){
                case 0x9E: // skip next instruction if key with value of Vx is pressed
                    // uint8_t key = V[regX]; 
                    if(keypad[V[regX]] == 1){
                        pc += 2; 
                    }
                    break; 
                case 0xA1: // skip if key with value vx is not pressed 
                    if(keypad[V[regX]] == 0){
                        pc += 2;
                    }
                    break; 
            }
            break; 
        case 0xF000: 
            switch (byte) {
                case 0x07: // set V[regX] to timer value  
                    V[regX] = delay_timer; 
                    break;
                case 0x0A:{// If a key is pressed, set regX to it  Fx0A - blocking instruction. 
                    bool keyPressed = false; 
                    for(uint8_t i = 0; i < 16; ++i){
                        if(keypad[i]){
                            V[regX] = i; 
                            keyPressed = true; 
                            break; 
                        }
                    }
                    
                    if(!keyPressed){
                        pc -= 2; 
                    }
                    break; 
                }
                case 0x15:
                    delay_timer = V[regX];
                    break; 
                case 0x18:
                    sound_timer = V[regX];
                    break; 
                case 0x1E:
                    I += V[regX]; 
                    break; 
                case 0x29: // Load F, Vx -> fx29  -----  set I = location of sprite for digit vX 
                    // Vx is a hex 0-F and corresponds to the sprite in memory 
                    // Each font is 5 bytes, so we need to multiply by five -> Ex: 0x050 -> 0x055 -> 0x05A -> 0x05F
                    I =  FONT_START_ADDRESS + (5 * V[regX]); 
                    break;
                case 0x33: {
                    // BCD -> Binary coded decimal - store BCD of Vx in mem locations I, I + 1 and I + 2
                    // hundreds digit at I, tens digit at I+1 and ones digit at I+2
                    uint8_t value = V[regX]; 
                    // ones
                    memory[I + 2] = value % 10; 
                    value /= 10;
                    // tens
                    memory[I + 1] = value % 10; 
                    value /= 10; 
                    // hundreds
                    memory[I] = value % 10;

                    break; 
                }
                case 0x55: 
                    // store registers V0  through Vx starting at location I 
                    for (int i = 0; i <= regX; ++i){
                        memory[I + i] = V[i];
                    }
                    break; 
                case 0x65: 
                    // read registers FROM mem V0 through Vx starting at location I
                    for (int i = 0;i <= regX; ++i){
                        V[i] = memory[I + 1]; 
                    }
                    break;  
                default: 
                    std::cout << "Invalid 0xF000 instruction " << std::endl; 
                    break; 
                }
            break; 
        
        default: 
            std::cout << "Unrecognized opcode" << std::endl; 
            break; 
    }

    // decrement timers. 
    if (delay_timer > 0){
        --delay_timer;
    }
    
    if(sound_timer > 0){
        --sound_timer;
    }
}; 

// Create a switch case statement to mask off with binary AND the first number. 
// First nibble = type of instructions 
// X  -> second nibble -> looks up V0 through VF register / VX 
// Y  -> third nibble -> looks up V0 through VF / VY 
// N  -> fourth nibble -> 4-bit number 
// NN -> third and fourth 8 bit
// NNN -> second, third and fourth nibble. 12 bit
// Create variables for this -> current_opcode, nibble1, x_index, y_index, n_value

