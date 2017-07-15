#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>


unsigned char chip8_fontset[80] ={
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0xF0, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

struct chip8{
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char gfx[64][32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];

    bool drawFlag = false;
    bool running = true;

    typedef void (*Opcode)();
    Opcode opcode_list[35];
    SDL_Renderer *renderTarget;

    void initialize(SDL_Renderer *renderer){
        //Init registers and memory once
        pc = 0x200;
        opcode = 0;
        I = 0;
        sp = 0;
        delay_timer = 0;
        sound_timer = 0;

        for(int i = 0; i < 80; ++i){
            memory[i] = chip8_fontset[i];
        }
        for (int i = 0; i < 16; ++i){
            key[i] = V[i] = 0;
        }
        for (int i = 0; i < 64; ++i){
            for (int j = 0; j < 32; ++j){
                gfx[i][j] = 0;
            }
        }
        renderTarget = renderer;

        
    }

    void loadFile(const char* fileName){
        std::streampos size;
        char *memblock;
        std::ifstream input (fileName, std::ios::in|std::ios::binary|std::ios::ate);
        if(input.is_open()){
            size = input.tellg();
            memblock = new char[size];
            input.seekg(0,std::ios::beg);
            input.read(memblock,size);
        }
        for(int i = 0; i < size; ++i){
            memory[i+512] = memblock[i];
        }
        delete[] memblock;
    }

    void emulateCycle(){
        //Fetch Opcode
        opcode = memory[pc] << 8 | memory[pc+1];
        std::cout << opcode << std::endl;
        std::cout << "Program Counter :" << pc << std::endl;
        //Decode Opcode
        //Execute Opcode
        switch(opcode & 0xF000){
            //Execute OPcode
            case 0x0000:
                switch(opcode & 0x0FFF){
                    case 0x0FF0: op_00E0(); break;
                    case 0x0FFF: op_00EE(); break;
                    default:break;
                }break;
            case 0x2000:op_2NNN(); break;
            case 0x3000:op_3XNN(); break;
            case 0x4000:op_4XNN(); break;
            case 0x5000:op_5XYO(); break;
            case 0x6000:op_6XNN(); break;
            case 0x7000:op_7XNN(); break;
            case 0x8000:
                switch(opcode & 0x000F){
                    case 0x0000:op_8XY0(); break;
                    case 0x0001:op_8XY1(); break;
                    case 0x0002:op_8XY2(); break;
                    case 0x0003:op_8XY3(); break;
                    case 0x0004:op_8XY4(); break;
                    case 0x0005:op_8XY5(); break;
                    case 0x0006:op_8XY6(); break; 
                    case 0x0007:op_8XY7(); break;
                    case 0x000E:op_8XYE(); break;
                    default:break;
                }break;
            case 0x9000:op_9XY0();break;
            case 0xA000:op_ANNN();break;;
            case 0xB000:op_BNNN();break;
            case 0xC000:op_CXNN();break;
            case 0xD000:op_DXYN();break;
            case 0xE000:
                switch(opcode & 0x00FF){
                    case 0x009E:op_EX9E();break;
                    case 0x00A1:op_EXA1();break;
                    default:break;
                }break;
            case 0xF000:
                switch(opcode & 0x00FF){
                    case 0x0007:op_FX07();break;
                    case 0x000A:op_FX0A();break;
                    case 0x0015:op_FX15();break;
                    case 0x0018:op_FX18();break;
                    case 0x001E:op_FX1E();break;
                    case 0x0029:op_FX29();break;
                    case 0x0033:op_FX33();break;
                    case 0x0055:op_FX55();break;
                    case 0x0065:op_FX65();break;

                    default:break;
                }break;

            default:
                std::cout << "Unknown Opcode " << std::endl;
                break;

        }
        //Update Timers
        if(delay_timer > 0){
            --delay_timer;
        }
        if(sound_timer > 0){
            if(sound_timer == 1)
                printf("BEEP!\n");
            --sound_timer;
        }
    }

    void setKeys(){
        SDL_Event event;
        SDL_PollEvent(&event);
        if(event.type == SDL_KEYUP){
            switch(event.key.keysym.sym){
                case SDLK_1: key[0x1] = 1; break;
                case SDLK_2: key[0x2] = 1; break;
                case SDLK_3: key[0x3] = 1; break;
                case SDLK_4: key[0xC] = 1; break;
                case SDLK_q: key[0x4] = 1; break;
                case SDLK_w: key[0x5] = 1; break;
                case SDLK_e: key[0x6] = 1; break;
                case SDLK_r: key[0xD] = 1; break;
                case SDLK_a: key[0x7] = 1; break;
                case SDLK_s: key[0x8] = 1; break;
                case SDLK_d: key[0x9] = 1; break;
                case SDLK_f: key[0xE] = 1; break;
                case SDLK_z: key[0xA] = 1; break;
                case SDLK_x: key[0x0] = 1; break;
                case SDLK_c: key[0xB] = 1; break;
                case SDLK_v: key[0xF] = 1; break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:break;
            }
        }
    }

    void op_00E0(){
        SDL_SetRenderDrawColor(renderTarget,0,0,0,0);
        pc +=2;
    }
    void op_00EE(){
        pc = stack[sp];
        --sp;
    }
    void op_1NNN(){
        pc = opcode & 0x0FFF;
    }
    void op_2NNN(){
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
    }
    void op_3XNN(){
        if((V[(opcode & 0x0F00) >> 8]) == (opcode & 0x0FF0) >> 4){
            pc +=4;
        }
    }
    void op_4XNN(){
        if((V[opcode & 0x0F00] >> 8) != (opcode & 0x0FF0) >> 4){
            pc +=4;
        }
    }
    void op_5XYO(){
        if(((opcode & 0xF000) >> 12 ) == ((opcode & 0x0F00) >> 8)){
            pc +=4;
        }
    }
    void op_6XNN(){
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        pc += 2;
    }
    void op_7XNN(){
        V[(opcode & 0xF000) >> 12] += (opcode & 0x00FF);
        pc +=2;
    }
    void op_8XY0(){
        V[(opcode & 0x0F00) >> 8] = ((opcode & 0x00F0) >> 4);
        pc +=2;
    }
    void op_8XY1(){
        V[(opcode & 0x0F00) >> 8] = (((opcode & 0x0F00) >> 8) | ((opcode & 0x00F0) >> 4));
        pc +=2;
    }
    void op_8XY2(){
        V[(opcode & 0x0F00) >> 8] = (((opcode & 0x0F00) >> 8) & ((opcode & 0x00F0) >> 4));
        pc +=2;
    }
    void op_8XY3(){
        V[(opcode & 0x0F00) >> 8] = (((opcode & 0x0F00) >> 8) ^ ((opcode & 0x00F0) >> 4));
        pc +=2;
    }
    void op_8XY4(){
        if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
            V[0xF] = 1;
        }else{
            V[0xF] = 0;
        }
    }
    void op_8XY5(){
        if(((opcode & 0x00F0) >> 4) > V[(opcode & 0x0F00) >> 8]){
            V[0xF] = 0;
        }else{
            V[0xF] = 1;
        }
        V[(opcode & 0x0F00) >> 8] -= ((opcode & 0x00F0) >> 4);
        pc += 2;
    }
    void op_8XY6(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
        pc +=2;
    }
    void op_8XY7(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    void op_8XYE(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
        pc += 2;
    }
    void op_9XY0(){
        if(((opcode & 0x0F00) >> 8) != ((opcode & 0x00F0) >> 4)){
            pc +=4;
        }
    }
    void op_ANNN(){
        I = opcode & 0x0FFF;
        pc += 2;
    }
    void op_BNNN(){
        pc = V[0] + (opcode & 0x0FFF);
    }
    void op_CXNN(){
        // TODO: Sets VX to the result of a bitwise and operation on a random number 
        pc +=2;
    }
    void op_DXYN(){
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x009F;
        unsigned short pixel;

        V[0xF] = 0;
        for (int yline = 0; yline < height; yline++){
            pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++){
                if(gfx[x + xline][y + yline] == 1){
                    V[0xF] = 1;
                }
                gfx[x + xline][y + yline] ^= 1;
            }
        }
        drawFlag = true;
        pc += 2;
        /*V[0xF] = 0;
        for(int yline = 0; yline < height; yline ++){
            pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++){
                if(gfx[(x + xline + ((y + yline) * 64))] == 1){
                    V[0xF] = 1;
                }
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
        drawFlag = true;
        pc += 2;*/
    }
    void op_EX9E(){
        if(key[V[(opcode & 0x0F00) >> 8]] !=8){
            pc += 4;
        }else{
            pc +=2;
        }
    }
    void op_EXA1(){
        if(key[V[(opcode & 0x0F00) >> 8]] != 0){
            pc += 2;
        }else{
            pc += 4;
        }
    }
    void op_FX07(){
        V[(opcode & 0x0F00) >> 8] = delay_timer;
        pc += 2;
    }
    void op_FX0A(){
        setKeys();
        for(int i = 0; i < 16; i++){
            if(key[i] == i){
                V[(opcode & 0x0F00) >> 8] = key[i];
                key[i] = 0;
            }
        }
        pc +=2;
    }
    void op_FX15(){
        delay_timer = V[(opcode & 0x0F00) >> 8];
        pc +=2;
    }
    void op_FX18(){
        sound_timer = V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    void op_FX1E(){
        I += V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    void op_FX29(){
        I = V[(opcode & 0x0F00) >> 8] * 0x5;
        pc +=2;
    }
    void op_FX33(){
        memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
        memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
        memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
        pc += 2;
    }
    void op_FX55(){
        for (int i = 0; i < ((opcode & 0x0F00) >> 8); ++i){
            memory[I + i] = V[i];
        }
        pc +=2;
    }
    void op_FX65(){
        for (int i = 0; i < ((opcode & 0x0F00) >> 8); ++i){
            V[i] = memory[I + 1];
        }
        pc +=2;
    }
} ;