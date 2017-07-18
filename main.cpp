#include "chip8.h"
#undef main

chip8 MyChip8;

int main (int argc, char* argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Hello World!",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,640,320,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer *renderTarget = SDL_CreateRenderer(window,-1,0);
    MyChip8.initialize(renderTarget);
    MyChip8.loadFile(argv[1]);

    while(MyChip8.running){
        MyChip8.emulateCycle();
        MyChip8.setKeys();
        if(MyChip8.drawFlag){
            SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            rect.h = 0;
            rect.w = 0;
            for(int i = 0; i < 64; ++i){
                for(int j = 0; j < 32; ++j){
                        rect.x = i * 10;
                        rect.y = j * 10;
                        rect.w = 10;
                        rect.h = 10;
                        if(MyChip8.gfx[i][j] == 1){
                            SDL_SetRenderDrawColor(renderTarget,0,0,0,0);
                            SDL_RenderFillRect(renderTarget,&rect);
                            MyChip8.gfx[i][j] = 0;
                        }else if(MyChip8.gfx[i][j] == 0){
                            SDL_SetRenderDrawColor(renderTarget,255,255,255,255);
                            SDL_RenderFillRect(renderTarget,&rect);
                            MyChip8.gfx[i][j] = 0;
                        }
                    }
                }
            }
        //SDL_Delay(500);
        SDL_RenderPresent(renderTarget);
        }
    std::cout << "Hello World!" << std::endl; 
    return 0;
}

//http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/