#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <cstdint> 
#include <chrono>  
#include <random> 

const unsigned int FONT_START_ADDRESS = 0x50; 
const unsigned int FONTSET_SIZE = 80; // 80 bytes total array.
const unsigned int START_ADDRESS = 0x200; 
Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    initialize();

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U); 
};

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
        memory[FONT_START_ADDRESS + i] = fontset[i];
    }

    delay_timer = 0; 
    sound_timer = 0; 
}

bool Chip8::loadRom(const char * filename){
    initialize();

    std::ifstream rom_file(filename, std::ios::binary | std::ios::ate);

    if(!rom_file){
        std::cerr << "Failed to open the file."; 
        return false; 
    }
    std::streamsize rom_size = rom_file.tellg(); 
    std::cout << "Rom Size is: " << rom_size << std::endl; 
    rom_file.seekg(0, std::ios::beg); 

    std::vector<char> buffer(rom_size); 

    if(!rom_file.read(buffer.data(), rom_size)){
        std::cerr << "Failed to read the file.\n";
        return false; 
    } else{
        std::cout << "File read and added to buffer successfully." << std::endl;
    };

    if(rom_size < (4096 - 512)){
        for(int i = 0; i < buffer.size(); ++i){
            memory[START_ADDRESS + i] = buffer[i];
        }
    }

    rom_file.close();
    return true;
}


void Chip8::cycle() {
    opcode = (memory[pc] << 8 | memory[pc + 1]);
    uint16_t instruction_type = opcode & 0xF000;
    uint8_t regX = (opcode & 0x0F00) >> 8; 
    uint8_t regY = (opcode & 0x00F0) >> 4;
    uint8_t byte = (opcode & 0x00FF); 
    uint8_t lastBit = opcode & 0x000F;     

    pc += 2; 

    switch(instruction_type) {
        case 0x0000: 
            if(opcode == 0x00E0){ // 00E0 
                for(int i = 0; i < gfx.size(); ++i){
                    gfx[i] = 0; 
                }
            } else if(opcode == 0x00EE) { // 00EE   
                --sp; 
                pc = stack[sp];
            }
            break; 
        case 0x1000: // 1NNN
            pc = opcode & 0x0FFF; 
            break;
        case 0x2000: // 2NNN
            stack[sp] = pc; 
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 3XNN
            if(V[regX] == byte){
                pc += 2;
            }
            break; 
        case 0x4000: // 4XNN
            if(V[regX] != byte){
                pc += 2; 
            }
            break; 
        case 0x5000: // 5XY0
            if(V[regX] == V[regY]){
                pc += 2; 
            }
            break; 
        case 0x6000: // 6XNN
            V[regX] = byte; 
            break;
        case 0x7000: // 7XNN
            V[regX] += byte;
            break; 
        case 0x8000: 
            switch (lastBit){
                case 0: // 8XY0
                    V[regX] = V[regY]; 
                    break;
                case 1: // 8XY1
                    V[regX] = V[regX] | V[regY]; 
                    break;
                case 2: // 8XY2
                    V[regX] = V[regX] & V[regY]; // AND
                    break;
                case 3: // 8XY3
                    V[regX] = V[regX] ^ V[regY]; // XOR 
                    break; 
                case 4: { // 8XY4
                    uint16_t sum = V[regX] + V[regY];
                    if(sum > 255){ 
                        V[0xF] = 1; 
                    } else {
                        V[0xF] = 0;
                    }
                    uint8_t value = sum & 0xFF; 
                    V[regX] = value; 
                    break;
                }
                case 5: // 8XY5
                    if(V[regX] > V[regY]){
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0; 
                    }
                    V[regX] -= V[regY]; 
                    break;
                case 6: // 8XY6
                    V[0xF] = (V[regX] & 0x1); 
                    V[regX] >>= 1; 
                    break; 
                case 7: // 8XY7
                    if(V[regY] > V[regX]){
                        V[0xF] = 1; 
                    } else {
                        V[0xF] = 0; 
                    }
                    V[regX] = V[regY] - V[regX];
                    break; 
                case 0xE: // 8XYE
                    V[0xF] = (V[regX] & 0x80) >> 7;
                    V[regX] <<= 1; 
                    break;  
                default:
                    std::cout << "0x8000 code not invalid or not recognized " << std::endl; 
            }
            break; 
        case 0x9000: // 9XY0
            if(V[regX] != V[regY]){
                pc += 2;
            }
            break;  
        case 0xA000:  // ANNN
            I = opcode & 0x0FFF;
            break;
        case 0xB000: // BNNN
            pc = (opcode & 0x0FFF) + V[0]; 
            break; 
        case 0xC000: // CXNN
            V[regX] = randByte(randGen) & byte; 
            break; 
        case 0xD000: { // DXYN  
            uint8_t height = opcode & 0x000F; 
            uint8_t width = 8; 
    
            uint8_t xCord = V[regX];
            uint8_t yCord = V[regY]; 
            V[0xF] = 0; 
            
            // let's say it's Dx0003 -> height would be 3 so it executes 3 rows  
            for(unsigned int row = 0; row < height; ++row){
                // Starts from index register. 
                uint8_t sprite_byte = memory[I + row]; 
                // loop through each sprite_byte of 8 bits of width 
                for(unsigned int col = 0; col < width; ++col){
                    // bit mask leftmost bit and shifts 0,1,2,3...7 position. 0x80
                    uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
                    if (sprite_pixel){
                        uint32_t x = (xCord + col) % VIDEO_WIDTH;
                        uint32_t y = (yCord + row) % VIDEO_HEIGHT;
                        uint32_t* screen_pixel = &gfx[y * VIDEO_WIDTH + x];
                        if(*screen_pixel == 0xFFFFFFFF){
                            V[0xF] = 1; // collision
                        }
                        // Perform XOR over the screen pixel to turn it ON 
                        *screen_pixel ^= 0xFFFFFFFF;
                    }                  
                }
            }
            break;
        } 
        case 0xE000: 
            switch(byte){
                case 0x9E: // EX9E 
                    if(keypad[V[regX]] == 1){ 
                        pc += 2; 
                    }
                    break; 
                case 0xA1: // EXA1 
                    if(keypad[V[regX]] == 0){
                        pc += 2;
                    }
                    break; 
            }
            break; 
        case 0xF000: 
            switch (byte) {
                case 0x07: // FX07  
                    V[regX] = delay_timer; 
                    break;
                case 0x0A:{// FX0A
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
                case 0x15: // FX15
                    delay_timer = V[regX];
                    break; 
                case 0x18: // FX17
                    sound_timer = V[regX];
                    break; 
                case 0x1E: // FX1E
                    I += V[regX]; 
                    break; 
                case 0x29: // FX29 
                    // Each font is 5 bytes, so we need to multiply by five -> Ex: 0x050 -> 0x055 -> 0x05A -> 0x05F
                    I =  FONT_START_ADDRESS + (5 * V[regX]); 
                    break;
                case 0x33: { // FX33 
                    uint8_t value = V[regX]; 
                    memory[I + 2] = value % 10; // ones
                    value /= 10;
                    memory[I + 1] = value % 10; // tens
                    value /= 10; 
                    memory[I] = value % 10; // hundreds

                    break; 
                }
                case 0x55: // FX55
                    for (int i = 0; i <= regX; ++i){
                        memory[I + i] = V[i];
                    }
                    break; 
                case 0x65: // FX65
                    for (int i = 0;i <= regX; ++i){
                        V[i] = memory[I + i]; 
                    }
                    break;  
                default: 
                    std::cout << "Invalid 0xF000 instruction " << std::hex << opcode << std::endl; 
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
