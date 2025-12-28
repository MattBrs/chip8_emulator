#ifndef MAIN_H
#define MAIN_H

#include "stack.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>

#define MEMSIZE 4096
#define SCREEN_W 64
#define SCREEN_H 32
#define SCALE 8
#define FONT_MEMORY_LOCATION 0x050
#define FONTSET_SIZE 80

static bool legacy_mode = true;
static bool screen_state[SCREEN_H][SCREEN_W] = {0};
const uint8_t fonts[FONTSET_SIZE] = {
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
static int16_t index_register; // index register
static uint8_t v[16];          // general purpose variables registers
static Stack functions_stack;  // functions / subroutines stack
static uint8_t delay_timer;    // decremented at rate of 60hz until 0
static uint8_t audio_timer;    // like delay_timer, beeps at numbers != 0
static bool keyboard[17];

const double CPU_INTERVAL = 1.0 / 700;
const double TIMER_INTERVAL = 1.0 / 60;
static double cpu_accumulator = 0.0;
static double timer_accumulator = 0.0;

static uint64_t last_time = 0.0;
static double frequency = 0.0;
static int current_sine_sample = 0;

// SDL functions
void close_sdl(SDL_Window *window, SDL_Renderer *renderer);
void render(SDL_Renderer *renderer);
void set_pixel_color(SDL_Surface *surface, const int x, const int y,
                     const uint32_t color);
SDL_Texture *get_screen_texture(SDL_Renderer *renderer, const int screen_w,
                                const int screen_h);

void handle_audio(SDL_AudioStream *stream);

// utils
uint8_t *byte_to_bits(const uint8_t byte, uint8_t *bits_arr);

// emulator generic functions
void init_emulator(
    char *rom_name); // loads stuff into memory and bootstraps the system
void load_program(char *program_file_path);
bool execute_cycle();
void handle_input(bool *running);

// emulator opetaion functions
void op_clear_screen();
void op_jump(uint16_t dst);
void op_return_subroutine();
void op_set_index(uint16_t value);
void op_set(uint8_t reg1, uint8_t reg2);
void op_call_subroutine(uint16_t function);
void op_binary_or(uint8_t reg1, uint8_t reg2);
void op_binary_and(uint8_t reg1, uint8_t reg2);
void op_binary_xor(uint8_t reg1, uint8_t reg2);
void op_skip_eq_reg(uint8_t reg1, uint8_t reg2);
void op_set_register(uint8_t reg, uint8_t value);
void op_add_registers(uint8_t reg1, uint8_t reg2);
void op_skip_eq_reg_num(uint8_t reg, uint8_t value);
void op_skip_not_eq_reg(uint8_t reg1, uint8_t reg2);
void op_add_to_register(uint8_t reg, uint8_t value);
void op_vy_minus_vx(uint8_t reg1, uint8_t reg2);
void op_vx_minus_vy(uint8_t reg1, uint8_t reg2);
void op_skip_not_eq_reg_num(uint8_t reg, uint8_t value);
void op_draw_sprite(uint8_t reg1, uint8_t reg2, uint8_t n);
void op_shift_right(uint8_t reg1, uint8_t reg2);
void op_shift_left(uint8_t reg1, uint8_t reg2);
void op_jump_with_offset(uint8_t reg1, uint8_t nn, uint16_t nnn);
void op_random(uint8_t reg1, uint8_t nn);
void op_skip_if_key(uint8_t reg1);
void op_skip_if_not_key(uint8_t reg1);
void op_set_reg_to_delay_timer(uint8_t reg);
void op_set_delay_timer_to_reg(uint8_t reg);
void op_set_sound_timer_to_reg(uint8_t reg);
void op_add_to_index(uint8_t reg);
void op_get_key(uint8_t reg);
void op_set_font_char(uint8_t reg);
void op_decode_to_decimal(uint8_t reg);
void op_store_memory(uint8_t reg);
void op_load_memory(uint8_t reg);

#endif // !MAIN_H
