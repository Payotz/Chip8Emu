#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <cstdlib>


unsigned char chip8_fontset[80] ={
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
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
    unsigned char oldgfx[64][32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char key[16];

    bool drawFlag = false;
    bool getKeys = false;
    bool running = true;

    typedef void (*Opcode)();
    Opcode opcode_list[35];

    void initialize(){
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
            stack[i] = 0;
        }
        for (int i = 0; i < 16; ++i){
            key[i] = V[i] = 0;
        }
        for (int i = 0; i < 64; ++i){
            for (int j = 0; j < 32; ++j){
                gfx[i][j] = oldgfx[i][j] = 0;
            }
        }
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
        std::cout << "Reading in file : " << fileName << std::endl;
        std::cout << "Size is : " << size << std::endl;
        for(int i = 0; i < size; ++i){
            memory[i+512] = memblock[i];
        }
        delete[] memblock;
    }

    void emulateCycle(){
        opcode = memory[pc] << 8 | memory[pc+1];
        switch(opcode & 0xF000){
            case 0x0000:
                switch(opcode & 0x000F){
                    case 0x0000: op_00E0(); break;
                    case 0x000E: op_00EE(); break;
                    default:
                        std::cout << "Opcode not found : ( " << opcode << " ) at Memory Location : (" << pc << ")" << std::endl;
                        break;
                }break;
            case 0x1000:op_1NNN(); break;
            case 0x2000:op_2NNN(); break;
            case 0x3000:op_3XNN(); break;
            case 0x4000:op_4XNN(); break;
            case 0x5000:op_5XY0(); break;
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
                    default:
                        std::cout << "Opcode not found : ( " << opcode << " ) at Memory Location : (" << pc << ")" << std::endl;
                        break;
                }break;
            case 0x9000:op_9XY0();break;
            case 0xA000:op_ANNN();break;
            case 0xB000:op_BNNN();break;
            case 0xC000:op_CXNN();break;
            case 0xD000:op_DXYN();break;
            case 0xE000:
                switch(opcode & 0x00FF){
                    case 0x009E:op_EX9E();break;
                    case 0x00A1:op_EXA1();break;
                    default:
                        std::cout << "Opcode not found : ( " << opcode << " ) at Memory Location : (" << pc << ")" << std::endl;
                        break;
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

                    default:
                        std::cout << "Opcode not found : ( " << opcode << " ) at Memory Location : (" << pc << ")" << std::endl;
                        break;
                }break;

            default:
                    std::cout << "Opcode not found : ( " << opcode << " ) at Memory Location : (" << pc << ")" << std::endl;
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

    bool setKeys(){
        SDL_Event event;
        SDL_PollEvent(&event);
        if(event.type == SDL_KEYDOWN){
            switch(event.key.keysym.sym){
                case SDLK_1: key[0x1] = 1; return true; break;
                case SDLK_2: key[0x2] = 1; return true; break;
                case SDLK_3: key[0x3] = 1; return true; break;
                case SDLK_4: key[0xC] = 1; return true; break;
                case SDLK_q: key[0x4] = 1; return true; break;
                case SDLK_w: key[0x5] = 1; return true; break;
                case SDLK_e: key[0x6] = 1; return true; break;
                case SDLK_r: key[0xD] = 1; return true; break;
                case SDLK_a: key[0x7] = 1; return true; break;
                case SDLK_s: key[0x8] = 1; return true; break;
                case SDLK_d: key[0x9] = 1; return true; break;
                case SDLK_f: key[0xE] = 1; return true; break;
                case SDLK_z: key[0xA] = 1; return true; break;
                case SDLK_x: key[0x0] = 1; return true; break;
                case SDLK_c: key[0xB] = 1; return true; break;
                case SDLK_v: key[0xF] = 1; return true; break;
                case SDLK_ESCAPE:
                    running = false;
                    return true;
                    break;
                default: return false; break;
            }
        }else{
            switch(event.key.keysym.sym){
                case SDLK_1: key[0x1] = 0; return true; break;
                case SDLK_2: key[0x2] = 0; return true; break;
                case SDLK_3: key[0x3] = 0; return true; break;
                case SDLK_4: key[0xC] = 0; return true; break;
                case SDLK_q: key[0x4] = 0; return true; break;
                case SDLK_w: key[0x5] = 0; return true; break;
                case SDLK_e: key[0x6] = 0; return true; break;
                case SDLK_r: key[0xD] = 0; return true; break;
                case SDLK_a: key[0x7] = 0; return true; break;
                case SDLK_s: key[0x8] = 0; return true; break;
                case SDLK_d: key[0x9] = 0; return true; break;
                case SDLK_f: key[0xE] = 0; return true; break;
                case SDLK_z: key[0xA] = 0; return true; break;
                case SDLK_x: key[0x0] = 0; return true; break;
                case SDLK_c: key[0xB] = 0; return true; break;
                case SDLK_v: key[0xF] = 0; return true; break;

                default: return false; break;
            }
        }
    }

    // Clears the screen.
    void op_00E0(){
        for (int i = 0; i < 64; i++){
            for(int j = 0; j < 32; j++){
                gfx[i][j] = 0;
            }
        }
        drawFlag = true;
        pc +=2;
    }
    // Returns from a subroutine.
    void op_00EE(){
        --sp;
        pc = stack[sp];
        pc +=2;
    }
    //Jumps to address NNN.
    void op_1NNN(){
        pc = opcode & 0x0FFF;
    }
    // Calls subroutine at NNN.
    void op_2NNN(){
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
    }
    // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
    void op_3XNN(){
        if((V[(opcode & 0x0F00) >> 8]) == (opcode & 0x00FF) ){
            pc +=4;
        }else{
            pc +=2;
        }
    }
    // Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
    void op_4XNN(){
        if((V[opcode & 0x0F00 >> 8]) != (opcode & 0x00FF)){
            pc +=4;
        }else{
            pc +=2;
        }
    }
    // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
    void op_5XY0(){
        if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
            pc +=4;
        }else{
            pc +=2;
        }
    }
    // Sets VX to NN.
    void op_6XNN(){
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        pc += 2;
    }
    // Adds NN to VX.
    void op_7XNN(){
        V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        pc +=2;
    }
    // Sets VX to the value of VY.
    void op_8XY0(){
        V[(opcode & 0x0F00) >> 8] = ((opcode & 0x00F0) >> 4);
        pc +=2;
    }
    // Sets VX to VX or VY. (Bitwise OR operation)
    void op_8XY1(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
        pc +=2;
    }
    // Sets VX to VX and VY. (Bitwise AND operation)
    void op_8XY2(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
        pc +=2;
    }
    // Sets VX to VX xor VY.
    void op_8XY3(){
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
        pc +=2;
    }
    // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
    void op_8XY4(){
        if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
            V[0xF] = 1;
        }else{
            V[0xF] = 0;
        }
        V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
        pc +=2;
    }
    // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    void op_8XY5(){
        if(((opcode & 0x00F0) >> 4) > (0xFF - V[(opcode & 0x0F00) >> 8])){
            V[0xF] = 0;
        }else{
            V[0xF] = 1;
        }
        V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
        pc += 2;
    }
    // Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
    void op_8XY6(){
        V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1; 
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
        pc +=2;
    }
    // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    void op_8XY7(){
        if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]){
            V[0xF] = 1;
        } else{
            V[0xF] = 0;
        }
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    // Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
    void op_8XYE(){
        V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
        pc += 2;
    }
    // Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
    void op_9XY0(){
        if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
            pc +=4;
        }
        pc +=2;
    }
    // Sets I to the address NNN.
    void op_ANNN(){
        I = opcode & 0x0FFF;
        pc += 2;
    }
    // Jumps to the address NNN plus V0.
    void op_BNNN(){
        pc = (opcode & 0x0FFF) + V[0];
    }
    // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    void op_CXNN(){
        V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x0FF);
        pc +=2;
    }
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
    // Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. 
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
    void op_DXYN(){
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x000F;
        unsigned short pixel;

        V[0xF] = 0;
        for (int i =0; i < 64; i++){
            for (int j = 0; j < 32; j++){
                oldgfx[i][j] = gfx[i][j];
            }
        }

        for (int yline = 0; yline < height; yline++){
            pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++){
                if((pixel & (0x80 >> xline)) !=0){
                    if(gfx[x + xline][y + yline] == 1){
                        V[0xF] = 1;
                    }
                    gfx[x + xline][y + yline] ^= 1;
                }
            }
        }
        drawFlag = true;
        pc += 2;
    }
    // Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
    void op_EX9E(){
        if(key[V[(opcode & 0x0F00) >> 8]] !=0){
            pc += 4;
        }else{
            pc += 2;
        }
    }
    // Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
    void op_EXA1(){
        if(key[V[(opcode & 0x0F00) >> 8]] == 0){
            pc += 2;
        }else{
            pc += 4;
        }
    }
    // Sets VX to the value of the delay timer.
    void op_FX07(){
        V[(opcode & 0x0F00) >> 8] = delay_timer;
        pc += 2;
    }
    // A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
    void op_FX0A(){
        bool keyPress = false;
        for(int i = 0; i < 16; i++){
            if(key[i] != 0){
                V[(opcode & 0x0F00) >> 8] = i;
                keyPress = true;
            }
            if(!keyPress){
                return;
            }
            pc +=2;
        }
    }
    // Sets the delay timer to VX.
    void op_FX15(){
        delay_timer = V[(opcode & 0x0F00) >> 8];
        pc +=2;
    }
    // Sets the sound timer to VX.
    void op_FX18(){
        sound_timer = V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    // Adds VX to I.
    void op_FX1E(){
        if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF){
            V[0xF] = 1;
        }else{
            V[0xF] = 0;
        }
        I += V[(opcode & 0x0F00) >> 8];
        pc += 2;
    }
    // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    void op_FX29(){
        I = V[(opcode & 0x0F00) >> 8] * 0x5;
        pc +=2;
    }
    // Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, 
    // the middle digit at I plus 1, and the least significant digit at I plus 2. 
    // (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, 
    // and the ones digit at location I+2.)
    void op_FX33(){
        memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
        memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
        memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
        pc += 2;
    }
    // Stores V0 to VX (including VX) in memory starting at address I
    void op_FX55(){
        for (int i = 0; i < ((opcode & 0x0F00) >> 8); ++i){
            memory[I + i] = V[i];
        }
        I += ((opcode & 0x0F00) >> 8 ) + 1;
        pc +=2;
    }
    // Fills V0 to VX (including VX) with values from memory starting at address I
    void op_FX65(){
        for (int i = 0; i < ((opcode & 0x0F00) >> 8); ++i){
            V[i] = memory[I + 1];
        }
        
        pc +=2;
    }
} ;