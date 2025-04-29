#include "GraphicController.h"
#include <iostream> 
#include <SDL2/SDL.h> 

GraphicController::GraphicController(const char* title, int winHeight, int winWidth){
    SDL_Init(SDL_INIT_EVERYTHING); // SDL_INIT_VIDEO
    // title, x, y, w, h, flags 
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_RESIZABLE); //SDL_WINDOW_SHOWN
    // window, index (-1 is auto), flags -> GPU resources hardware acc
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    // renderer, format (32 bits per pixel), access type(textures updated frequently each frame), w, height
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    // TODO = Error handling 
}

GraphicController::~GraphicController(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window);
    SDL_Quit();    
}

// gfx is the buffer 
void GraphicController::update(const void* gfx, int pitch){
    SDL_UpdateTexture(texture, NULL, gfx, pitch);  
    SDL_RenderClear(renderer);            
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);    
}


bool GraphicController::handleInput(uint8_t* keys){
    SDL_Event e; 

}