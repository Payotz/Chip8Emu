#include "chip8.h"
#undef main

chip8 MyChip8;

int main (int argc, char* argv[]){
    int previousTime = 0;
    int currentTime = 0;
    int fps = 0;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Chip8 Boiz",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,640,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer *renderTarget = SDL_CreateRenderer(window,-1,0);
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);

    MyChip8.initialize();
    MyChip8.loadFile(argv[1]);

    while(MyChip8.running){
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        fps++;

        MyChip8.emulateCycle();
        MyChip8.setKeys();
        if(MyChip8.drawFlag){
            SDL_Rect rect;
            rect.x = rect.y = rect.h = rect.w = 0;
            for(int i = 0; i < 128; ++i){
                for(int j = 0; j < 64; ++j){
                    if(MyChip8.extendedMode){
                        rect.x = i * 10;
                        rect.y = j * 10;
                        rect.w = 10;
                        rect.h = 10;
                        if(MyChip8.gfx[i][j] == 1){
                            SDL_SetRenderDrawColor(renderTarget,255,255,255,255);
                            SDL_RenderFillRect(renderTarget,&rect);
                            //MyChip8.gfx[i][j] = 0;
                        }else if(MyChip8.gfx[i][j] == 0){
                            SDL_SetRenderDrawColor(renderTarget,0,0,0,255);
                            SDL_RenderFillRect(renderTarget,&rect);
                            //MyChip8.gfx[i][j] = 0;
                        }
                    }
                    else{
                        rect.x = i * 2 * 10;
                        rect.y = j * 2 * 10;
                        rect.w = 10 * 2;
                        rect.h = 10 * 2;
                        if(MyChip8.gfx[i][j] == 1){
                            SDL_SetRenderDrawColor(renderTarget,255,255,255,255);
                            SDL_RenderFillRect(renderTarget,&rect);
                            //MyChip8.gfx[i][j] = 0;
                        }else if(MyChip8.gfx[i][j] == 0){
                            SDL_SetRenderDrawColor(renderTarget,0,0,0,255);
                            SDL_RenderFillRect(renderTarget,&rect);
                            //MyChip8.gfx[i][j] = 0;
                        }
                    }
                }
            }
        }
        if(((currentTime - previousTime) / 60) > 60){
            if(MyChip8.sound_timer > 0){
                MyChip8.sound_timer--;
            }else if(MyChip8.delay_timer > 0){
                MyChip8.delay_timer--;
            }
        }
        if(((currentTime - previousTime) / 1000) > 1000){
            SDL_Delay((currentTime - previousTime) / 1000);
        }


        SDL_RenderPresent(renderTarget);
        MyChip8.drawFlag = false;
        }
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderTarget);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

//http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/