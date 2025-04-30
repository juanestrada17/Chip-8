#pragma once 
#include <iostream> 
#include <SDL2/SDL.h> 
#include <array>

class GraphicsController{
public:
    GraphicsController(const char* title, int winHeight, int winWidth); 
    void update(const void* gfx, int pitch);  //const uint8_t* gfx,
    // destructor 
    ~GraphicsController();
    bool handleInput(std::array<uint8_t, 16>& keys);
private: 
    SDL_Window* window;
    SDL_Renderer* renderer; 
    SDL_Texture* texture; 
};
