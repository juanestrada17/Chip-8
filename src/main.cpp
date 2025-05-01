#define SDL_MAIN_HANDLED
#include<iostream> 
#include "GraphicsController.h"
#include "Chip8.h"
#include <SDL2/SDL.h> 
#include <chrono> 

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "Usage: .exe romname" << std::endl; 
        return 1; 
    }
    
    const char* romFilename = argv[1]; 
    // init graphics 
    GraphicsController gfx("Chip-8 Emulator", VIDEO_WIDTH * 10, VIDEO_HEIGHT * 20); 
    // create chip8 object 
    Chip8 chip8; 

    if (!chip8.loadRom(romFilename)) {
        std::cerr << "Failed to load ROM: " << romFilename << std::endl;
        return 1;
    }
    
    int videoPitch = sizeof(uint32_t) * VIDEO_WIDTH; 
    float cycleDelay = 1000.0f / 500.0f;
    auto lastCycleTime = std::chrono::high_resolution_clock::now(); 
    bool quit = false;

    while(!quit){
        quit = gfx.handleInput(chip8.keypad); 
        auto currentTime = std::chrono::high_resolution_clock::now(); 
        float elapsedTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if(elapsedTime > cycleDelay){
            lastCycleTime = currentTime; 
            
            chip8.cycle();

            gfx.update(chip8.gfx, videoPitch);
        }
    }
    return 0; 
}