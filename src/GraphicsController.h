#pragma once 

#define SDL_MAIN_HANDLED
#include <iostream> 
#include <SDL2/SDL.h> 

class GraphicsController{
    public:
        GraphicsController(const char* title, int winHeight, int winWidth); 
        void update(const void* gfx, int pitch);  //const uint8_t* gfx,
        // destructor 
        ~GraphicsController();
        bool handleInput(uint8_t* keys);
    private: 
        SDL_Window* window;
        SDL_Renderer* renderer; 
        SDL_Texture* texture; 
}
