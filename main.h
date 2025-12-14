#ifndef MAIN_H
#define MAIN_H

#include "stack.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>

#define MEMSIZE 4096
#define SCREEN_W 64
#define SCREEN_H 32
#define SCALE 10

static bool screen_state[SCREEN_H][SCREEN_W] = {0};

const uint8_t fonts[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static uint8_t memory[MEMSIZE];
static int16_t program_counter;
static int16_t index_register;             // index register
static int8_t v[16];          // general purpose variables registers
static Stack functions_stack; // functions / subroutines stack
static int8_t delay_timer;    // decremented at rate of 60hz until 0
static int8_t audio_timer;    // like delay_timer, beeps at numbers != 0

// SDL functions
void close_sdl(SDL_Window *window, SDL_Renderer *renderer);
void render(SDL_Renderer *renderer);
void set_pixel_color(SDL_Surface *surface, const int x, const int y,
                     const uint32_t color);
SDL_Texture *get_screen_texture(SDL_Renderer *renderer, const int screen_w,
                                const int screen_h);


// utils
uint8_t* byte_to_bits(const uint8_t byte, uint8_t* bits_arr);

// emulator generic functions
void init_emulator(char* rom_name); // loads stuff into memory and bootstraps the system
void load_program(char* program_file_path);
bool execute_cycle();


// emulator opetaion functions
void op_dxyn();
void op_clear_screen();
void op_jump(uint16_t dst);
void op_set_index(uint16_t value);
void op_add_register(uint8_t reg, uint8_t value);
void op_set_register(uint8_t reg, uint8_t value);
void op_draw_sprite(uint8_t reg1, uint8_t reg2, uint8_t n);

#endif // !MAIN_H
