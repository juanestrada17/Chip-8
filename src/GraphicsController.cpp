#include "GraphicsController.h"
#include <iostream> 
#include <SDL2/SDL.h> 
#include <array>

GraphicsController::GraphicsController(const char* title, int winHeight, int winWidth){
    SDL_Init(SDL_INIT_EVERYTHING); // SDL_INIT_VIDEO
    // title, x, y, w, h, flags 
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN); //SDL_WINDOW_SHOWN
    // window, index (-1 is auto), flags -> GPU resources hardware acc
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    // renderer, format (32 bits per pixel), access type(textures updated frequently each frame), w, height
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
};

GraphicsController::~GraphicsController(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window);
    SDL_Quit();    
};

// gfx / buffer 
void GraphicsController::update(std::array<uint32_t, 64 * 32>& gfx, int pitch){
    SDL_UpdateTexture(texture, NULL, gfx.data(), pitch);  
    SDL_RenderClear(renderer);            
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);    
};

// Input handler 
bool GraphicsController::handleInput(std::array<uint8_t, 16>& keys){
    bool quit = false; 

    SDL_Event e;
    
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT){
            quit = true;
        }
        // key presses 
        else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
            
            bool keyPressed = e.type == SDL_KEYDOWN;

            switch(e.key.keysym.sym){
                case SDLK_ESCAPE: 
                    quit = true; 
                    break;
                case SDLK_1:
                    keys[1] = keyPressed; 
                    break;
                case SDLK_2: 
                    keys[2] = keyPressed; 
                    break;
                case SDLK_3: 
                    keys[3] = keyPressed; 
                    break;
                case SDLK_4: 
                    keys[0xC] = keyPressed;
                    break;
                case SDLK_q:
                    keys[4] = keyPressed;
                    break;
                case SDLK_w:
                    keys[5] = keyPressed;
                    break;
                case SDLK_e:
                    keys[6] = keyPressed;
                    break;
                case SDLK_r:
                    keys[0xD] = keyPressed;
                    break;
                case SDLK_a:
                    keys[7] = keyPressed;
                    break;
                case SDLK_s:
                    keys[8] = keyPressed;
                    break;
                case SDLK_d:
                    keys[9] = keyPressed;
                    break;               
                case SDLK_f:
                    keys[0xE]= keyPressed;
                    break;               
                case SDLK_z:
                    keys[0xA] = keyPressed;
                    break;               
                case SDLK_x:
                    keys[0] = keyPressed;
                    break;               
                case SDLK_c:
                    keys[0xB] = keyPressed;
                    break;               
                case SDLK_v:
                    keys[0xF] = keyPressed;
                    break;                             
                default: 
                    std::cout << "Key not recognized" << std::endl;
                    break; 
            }
        }
    }
    return quit; 
};