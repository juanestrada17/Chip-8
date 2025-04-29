#pragma once

#include <iostream> 
#include <array>
#include <vector> 
#include <cstdint>  

// 700 instructions per second  -> good for most chip 8 programs 

class Chip8{
public: 
    Chip8(); 
    void initialize();
    void loadRom(); //copies the program in memory, loops through it and sets it into the memory array. 
private:
    std::array<uint8_t, 4096> memory; //4KB
    

    std::array<uint16_t, 16> stack; // 16 bit addresses, remembers current loc before a jump 
    uint8_t sp; 
    std::array<uint8_t, 16> V; // 8 bit, 16 registers V0 - VF  // V[0] - V[15]
    uint16_t pc; 
    uint16_t I; // index register

    uint8_t delay_timer;
    uint8_t sound_timer; 

    // 2048 pixels 
    std::array<uint8_t, 64 * 32> gfx;

    // 35 opcodes of two bytes
    uint16_t opcode; 

    // FDE 
    void cycle();

    // other instructions  
    void halt(); 


    // missing 
    // keypad, delaytimer, sound timer and graphics
    
}