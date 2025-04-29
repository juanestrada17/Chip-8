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

// gfx / buffer 
void GraphicController::update(const void* gfx, int pitch){
    SDL_UpdateTexture(texture, NULL, gfx, pitch);  
    SDL_RenderClear(renderer);            
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);    
}

// Input handler 
bool GraphicController::handleInput(uint8_t* keys){
    bool quit = false; 

    SDL_Event e;
    
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            quit = true;
        }
        //key presses 
        else if(e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                case SDLK_ESCAPE: 
                    quit = true; 
                    break;
                case SDLK_1:
                    keys[1] = 1; 
                    break;
                case SDLK_2: 
                    keys[2] = 1; 
                    break;
                case SDLK_3: 
                    keys[3] = 1; 
                    break;
                case SDLK_4: 
                    keys[0xC] = 1;
                    break;
                case SDLK_q:
                    keys[4] = 1;
                    break;
                case SDLK_w:
                    keys[5] = 1;
                    break;
                case SDLK_e:
                    keys[6] = 1;
                    break;
                case SDLK_r:
                    keys[0xD] = 1;
                    break;
                case SDLK_a:
                    keys[7] = 1;
                    break;
                case SDLK_s:
                    keys[8] = 1;
                    break;
                case SDLK_d:
                    keys[9] = 1;
                    break;               
                case SDLK_f:
                    keys[0xE]= 1;
                    break;               
                case SDLK_z:
                    keys[0xA] = 1;
                    break;               
                case SDLK_x:
                    keys[0] = 1;
                    break;               
                case SDLK_c:
                    keys[0xB] = 1;
                    break;               
                case SDLK_v:
                    keys[0xF] = 1;
                    break;                             
                default: 
                    std::cout << "Key not recognized" << std::endl;
                    break; 
            }
        }

        else if(e.type == SDL_KEYUP){
            switch (e.key.keysym.sym){
                case SDLK_1:
                    keys[1] = 0; 
                    break;
                case SDLK_2: 
                    keys[2] = 0; 
                    break;
                case SDLK_3: 
                    keys[3] = 0; 
                    break;
                case SDLK_4: 
                    keys[0xC] = 0;
                    break;
                case SDLK_q:
                    keys[4] = 0;
                    break;
                case SDLK_w:
                    keys[5] = 0;
                    break;
                case SDLK_e:
                    keys[6] = 0;
                    break;
                case SDLK_r:
                    keys[0xD] = 0;
                    break;
                case SDLK_a:
                    keys[7] = 0;
                    break;
                case SDLK_s:
                    keys[8] = 0;
                    break;
                case SDLK_d:
                    keys[9] = 0;
                    break;               
                case SDLK_f:
                    keys[0xE]= 0;
                    break;               
                case SDLK_z:
                    keys[0xA] = 0;
                    break;               
                case SDLK_x:
                    keys[0] = 0;
                    break;               
                case SDLK_c:
                    keys[0xB] = 0;
                    break;               
                case SDLK_v:
                    keys[0xF] = 0;
                    break;                             
                default: 
                    std::cout << "Key not recognized" << std::endl;
                    break; }
        }
    return quit; 
    }
}