#pragma once

#include <iostream> 
#include <array>
#include <vector> 
#include <cstdint>  
#include <random> 

const unsigned int MEMORY_SIZE = 4096; 
const unsigned int STACK_ADDRESSES = 16;
const unsigned int REGISTERS = 16;
const unsigned int VIDEO_HEIGHT = 32; 
const unsigned int VIDEO_WIDTH = 64; 
const unsigned int KEY_COUNT = 16; 

class Chip8{
public: 
    Chip8(); 
    void initialize();
    bool loadRom(const char* filename);
    void cycle();
    std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> gfx; // 2048 pixels 
    std::array<uint8_t, KEY_COUNT> keypad; 
private:
    std::array<uint8_t, MEMORY_SIZE> memory; //4KB
    

    std::array<uint16_t, STACK_ADDRESSES> stack;
    uint8_t sp; // stack pointer 
    std::array<uint8_t, REGISTERS> V; // 8 bit, 16 registers V0 - VF  // V[0] - V[15]
    uint16_t pc; // program counter
    uint16_t I; // index register

    uint8_t delay_timer;
    uint8_t sound_timer; 

    uint16_t opcode; // 35 opcodes of two bytes

    // rand 
    std::default_random_engine randGen; 
    std::uniform_int_distribution<uint8_t> randByte; 
};