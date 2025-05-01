#pragma once 
#include <iostream> 
#include <SDL2/SDL.h> 
#include <array>

class GraphicsController{
public:
    GraphicsController(const char* title, int winHeight, int winWidth); 
    void update(std::array<uint32_t, 64 * 32>& gfx, int pitch);  
    // destructor 
    ~GraphicsController();
    bool handleInput(std::array<uint8_t, 16>& keys);
private: 
    SDL_Window* window;
    SDL_Renderer* renderer; 
    SDL_Texture* texture; 
};
