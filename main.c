#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stack.h"

int main() {
  stack_init(&functions_stack, 128);
  // init_emulator();

  SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer("Chip-8 Emulator", SCREEN_W * SCALE,
                              SCREEN_H * SCALE, SDL_WINDOW_OPENGL, &window,
                              &renderer);

  if (window == NULL || renderer == NULL) {
    printf("could not init window or renderer!\n");
    close_sdl(window, renderer);
    return 1;
  }

  SDL_SetRenderLogicalPresentation(renderer, SCREEN_W, SCREEN_H,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  init_emulator();
  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    execute_cycle();
    render(renderer);
    SDL_Delay(1000 / 60);
    // running = false;
  }

  close_sdl(window, renderer);

  printf("bye bye!\n");
  return 0;
}

void render(SDL_Renderer *renderer) {
  SDL_Texture *screen_texture =
      get_screen_texture(renderer, SCREEN_W, SCREEN_H);

  SDL_RenderClear(renderer);
  SDL_RenderTexture(renderer, screen_texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void close_sdl(SDL_Window *window, SDL_Renderer *renderer) {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }

  if (window) {
    SDL_DestroyWindow(window);
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  SDL_Quit();
}

void set_pixel_color(SDL_Surface *surface, const int x, const int y,
                     const uint32_t color) {
  if (surface == NULL) {
    return;
  }

  Uint8 *pixel = (Uint8 *)surface->pixels;
  pixel += (y * surface->pitch) + (x * sizeof(uint32_t));
  *((uint32_t *)pixel) = color;
}

SDL_Texture *get_screen_texture(SDL_Renderer *renderer, const int screen_w,
                                const int screen_h) {
  SDL_Surface *screen_surface =
      SDL_CreateSurface(SCREEN_W, SCREEN_H, SDL_PIXELFORMAT_RGBA8888);
  for (int i = 0; i < SCREEN_W; ++i) {
    for (int j = 0; j < SCREEN_H; ++j) {
      set_pixel_color(screen_surface, i, j, screen_state[i][j]);
    }
  }

  return SDL_CreateTextureFromSurface(renderer, screen_surface);
}

void init_emulator() {
  memcpy(memory + 0x050, fonts, 80); // copy fonts into mem
  program_counter = 0x200;

  load_program("ibm.ch8");
}

void load_program(char *program_file_path) {
  FILE *program_file = fopen(program_file_path, "r");
  if (program_file == NULL) {
    printf("error while opening file\n");
    return;
  }

  fseek(program_file, 0, SEEK_END);
  int fsize = ftell(program_file);
  fseek(program_file, 0, SEEK_SET);

  if (fsize == -1) {
    printf("error while reading the file size\n");
    return;
  }

  char *program = malloc(fsize + 1);
  if (program == NULL) {
    printf("error while allocating program memory\n");
    fclose(program_file);

    return;
  }

  fread(program, fsize, 1, program_file);
  int res = ferror(program_file);
  if (res != 0) {
    printf("error while reading the file\n");
    free(program);
    fclose(program_file);

    return;
  }

  fclose(program_file);

  memcpy(memory + 0x200, program, fsize);
  free(program);

  printf("the program has been loaded correctly\n");
}

bool execute_cycle() {
  bool should_update_screen = false;
  // run fetch, decode and execute
  int16_t op_code =
      (memory[program_counter] << 8) | memory[program_counter + 1];
  program_counter += 2;

  uint8_t op_type = (op_code & 0xF000) >> 12;
  uint8_t x = (op_code & 0x0F00) >> 8;
  uint8_t y = (op_code & 0x00F0) >> 4;
  uint8_t n = op_code & 0x000F;
  uint8_t nn = op_code & 0x00FF;
  uint16_t nnn = op_code & 0x0FFF;

  printf("op_type: %x\n", op_type);
  switch (op_type) {
  case 0x0:
    printf("clear screen\n");
    op_clear_screen();
    should_update_screen = true;
    break;
  case 0x1:
    printf("jump\n");
    op_jump(nnn);
    break;
  case 0x6:
    printf("set register \n");
    op_set_register(x, nn);
    break;
  case 0x7:
    printf("add value to  register \n");
    op_add_register(x, nn);
    break;
  case 0xA:
    printf("set index register\n");
    op_set_index(nnn);
    break;
  case 0xD:
    printf("draw screen\n");
    op_draw_sprite(x, y, n);
    should_update_screen = true;
    break;
  default:
    printf("undefined instruction: %x\n", op_type);
  }

  return should_update_screen;
}

void op_dxyn() {}

void op_jump(uint16_t dst) { program_counter = dst; }

void op_set_register(uint8_t reg, uint8_t value) { v[reg] = value; }

void op_add_register(uint8_t reg, uint8_t value) { v[reg] += value; }

void op_set_index(uint16_t value) { i = value; }

void op_draw_sprite(uint8_t reg1, uint8_t reg2, uint8_t n) {
  // sprites are always 1 byte large
  // n tells us the height (how many bytes to read)

  uint8_t pos_x = v[reg1];
  uint8_t pos_y = v[reg2];
}

void op_clear_screen() {
  for (int i = 0; i < SCREEN_W; ++i) {
    for (int j = 0; j < SCREEN_H; ++j) {
      screen_state[i][j] = 0x0;
    }
  }
}
