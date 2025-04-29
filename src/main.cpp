#define SDL_MAIN_HANDLED
#include<iostream> 
#include "GraphicsController.h"
#include "Chip8.h"
#include <SDL2/SDL.h> 


int main(int argc, char* argv[]){
    
    if(argc < 2){
        std::cout << "Usage: .exe romname" << std::endl; 
        return 1; 
    }
    // get rom 
    const char* romFilename = argv[1]; 
    // init graphics 
    GraphicsController gfx("Chip-8 Emulator", VIDEO_WIDTH,  VIDEO_HEIGHT); 
    // create chip8 object 
    Chip8 chip8; 
    chip8.loadRom(romFilename);     

    int videoPitch = sizeof(chip8.gfx[0] * VIDEO_WIDTH); 
    // 700 instructions per sec 
    float cycleDelay = 1000.0f / 700.0f;
    auto lastCycleTime = std::chrono::high_resolution_clock::now(); 
    bool quit = false;

    while(!quit){
        // handle input returns true or false 
        quit = gfx.handleInput(chip8.keypad); 
        auto currentTime = std::chrono::high_resolution_clock::now(); 
        float elapsedTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if(elapsedTime > cycleDelay){
            lastCycleTime = currentTime; 
            
            chip8.cycle();

            gfx.update(chip8.gfx, videoPitch)
        }

    }

    return 0; 
}