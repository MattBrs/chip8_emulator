#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "stack.h"

int main(int argc, char *argv[]) {
  stack_init(&functions_stack, 128);
  init_emulator(argc > 1 ? argv[1] : NULL);

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

  SDL_AudioSpec audio_spec;
  audio_spec.channels = 1;
  audio_spec.freq = 8000;
  audio_spec.format = SDL_AUDIO_F32;

  SDL_AudioStream *audio_stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
  if (!audio_stream) {
    printf("could not open audio stream\n");
    close_sdl(window, renderer);
    exit(1);
  }

  // SDL_ResumeAudioStreamDevice(audio_stream);

  bool running = true;
  while (running) {
    uint64_t current_time = SDL_GetPerformanceCounter();
    double delta_time = (current_time - last_time) / frequency;
    last_time = current_time;

    cpu_accumulator += delta_time;
    timer_accumulator += delta_time;

    handle_input(&running);
    handle_audio(audio_stream);

    while (cpu_accumulator >= CPU_INTERVAL) {
      bool should_update_screen = execute_cycle();
      if (should_update_screen) {
        render(renderer);
      }

      cpu_accumulator -= CPU_INTERVAL;
    }

    while (timer_accumulator >= TIMER_INTERVAL) {
      if (delay_timer > 0) {
        printf("decreasing delay timer\n");
        --delay_timer;
      }

      if (audio_timer > 0) {
        printf("decreasing audio timer\n");
        SDL_ResumeAudioStreamDevice(audio_stream);
        --audio_timer;
      } else {
        SDL_PauseAudioStreamDevice(audio_stream);
      }

      timer_accumulator -= TIMER_INTERVAL;
    }
  }

  close_sdl(window, renderer);

  printf("bye bye!\n");
  return 0;
}

void handle_audio(SDL_AudioStream *stream) {
  const int minimum_audio =
      (8000 * sizeof(float)) /
      2; /* 8000 float samples per second. Half of that. */
  if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
    static float samples[512]; /* this will feed 512 samples each frame until we
                                  get to our maximum. */
    int i;

    /* generate a 440Hz pure tone */
    for (i = 0; i < SDL_arraysize(samples); i++) {
      const int freq = 440;
      const float phase = current_sine_sample * freq / 8000.0f;
      samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
      current_sine_sample++;
    }

    /* wrapping around to avoid floating-point errors */
    current_sine_sample %= 8000;

    /* feed the new data to the stream. It will queue at the end, and trickle
     * out as the hardware needs more data. */
    SDL_PutAudioStreamData(stream, samples, sizeof(samples));
  }
}

void handle_input(bool *running) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      *running = false;
      return;
    }

    if (event.type == SDL_EVENT_KEY_DOWN) {
      switch (event.key.key) {
      case SDLK_1:
        keyboard[0x1] = 1;
        break;
      case SDLK_2:
        keyboard[0x2] = 1;
        break;
      case SDLK_3:
        keyboard[0x3] = 1;
        break;
      case SDLK_4:
        keyboard[0xC] = 1;
        break;
      case SDLK_Q:
        keyboard[0x4] = 1;
        break;
      case SDLK_W:
        keyboard[0x5] = 1;
        break;
      case SDLK_E:
        keyboard[0x6] = 1;
        break;
      case SDLK_R:
        keyboard[0xD] = 1;
        break;
      case SDLK_A:
        keyboard[0x7] = 1;
        break;
      case SDLK_S:
        keyboard[0x8] = 1;
        break;
      case SDLK_D:
        keyboard[0x9] = 1;
        break;
      case SDLK_F:
        keyboard[0xE] = 1;
        break;
      case SDLK_Z:
        keyboard[0xA] = 1;
        break;
      case SDLK_X:
        keyboard[0x0] = 1;
        break;
      case SDLK_C:
        keyboard[0xB] = 1;
        break;
      case SDLK_V:
        keyboard[0xF] = 1;
        break;
      default:;
      }
    }

    if (event.type == SDL_EVENT_KEY_UP) {
      switch (event.key.key) {
      case SDLK_1:
        keyboard[0x1] = 0;
        break;
      case SDLK_2:
        keyboard[0x2] = 0;
        break;
      case SDLK_3:
        keyboard[0x3] = 0;
        break;
      case SDLK_4:
        keyboard[0xC] = 0;
        break;
      case SDLK_Q:
        keyboard[0x4] = 0;
        break;
      case SDLK_W:
        keyboard[0x5] = 0;
        break;
      case SDLK_E:
        keyboard[0x6] = 0;
        break;
      case SDLK_R:
        keyboard[0xD] = 0;
        break;
      case SDLK_A:
        keyboard[0x7] = 0;
        break;
      case SDLK_S:
        keyboard[0x8] = 0;
        break;
      case SDLK_D:
        keyboard[0x9] = 0;
        break;
      case SDLK_F:
        keyboard[0xE] = 0;
        break;
      case SDLK_Z:
        keyboard[0xA] = 0;
        break;
      case SDLK_X:
        keyboard[0x0] = 0;
        break;
      case SDLK_C:
        keyboard[0xB] = 0;
        break;
      case SDLK_V:
        keyboard[0xF] = 0;
        break;
      default:;
      }
    }
  }
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
  for (int i = 0; i < SCREEN_H; ++i) {
    for (int j = 0; j < SCREEN_W; ++j) {
      uint32_t pixel_color = screen_state[i][j] ? 0xFFFFFFFF : 0x0;
      set_pixel_color(screen_surface, j, i, pixel_color);
    }
  }

  return SDL_CreateTextureFromSurface(renderer, screen_surface);
}

void init_emulator(char *rom_name) {
  srand(time(NULL));
  memset(memory, 0, MEMSIZE);
  memset(v, 0, 16);
  memset(keyboard, false, 17);
  index_register = 0;

  memcpy(memory + FONT_MEMORY_LOCATION, fonts,
         FONTSET_SIZE); // copy fonts into mem
  program_counter = 0x200;

  load_program(rom_name != NULL ? rom_name : "roms/IBM_Logo.ch8");

  last_time = SDL_GetPerformanceCounter();
  frequency = (double)SDL_GetPerformanceFrequency();
}

void load_program(char *program_file_path) {
  FILE *program_file = fopen(program_file_path, "rb");
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

uint8_t *byte_to_bits(const uint8_t byte, uint8_t *bits_arr) {
  uint8_t mask = 0x80;

  for (int i = 0; i < 8; ++i) {
    bits_arr[i] = (byte & mask) != 0;
    mask >>= 1;
  }

  return bits_arr;
}

// run fetch, decode and execute
bool execute_cycle() {
  bool should_update_screen = false;
  uint16_t op_code = ((uint8_t)memory[program_counter] << 8) |
                     (uint8_t)memory[program_counter + 1];

  program_counter += 2;

  uint8_t op_type = (op_code & 0xF000) >> 12;
  uint8_t x = (op_code & 0x0F00) >> 8;
  uint8_t y = (op_code & 0x00F0) >> 4;
  uint8_t n = op_code & 0x000F;
  uint8_t nn = op_code & 0x00FF;
  uint16_t nnn = op_code & 0x0FFF;

  switch (op_type) {
  case 0x0:
    if (nn == 0xE0) {
      op_clear_screen();
      should_update_screen = true;
    } else if (nn == 0xEE) {
      op_return_subroutine();
    }
    break;
  case 0x1:
    op_jump(nnn);
    break;
  case 0x2:
    op_call_subroutine(nnn);
    break;
  case 0x3:
    op_skip_eq_reg_num(x, nn);
    break;
  case 0x4:
    op_skip_not_eq_reg_num(x, nn);
    break;
  case 0x5:
    op_skip_eq_reg(x, y);
    break;
  case 0x6:
    op_set_register(x, nn);
    break;
  case 0x7:
    op_add_to_register(x, nn);
    break;
  case 0x8:
    switch (n) {
    case 0x0:
      op_set(x, y);
      break;
    case 0x1:
      op_binary_or(x, y);
      break;
    case 0x2:
      op_binary_and(x, y);
      break;
    case 0x3:
      op_binary_xor(x, y);
      break;
    case 0x4:
      op_add_registers(x, y);
      break;
    case 0x5:
      op_vx_minus_vy(x, y);
      break;
    case 0x6:
      op_shift_right(x, y);
      break;
    case 0x7:
      op_vy_minus_vx(x, y);
      break;
    case 0xE:
      op_shift_left(x, y);
      break;
    default:;
    }
    break;
  case 0x9:
    op_skip_not_eq_reg(x, y);
    break;
  case 0xA:
    op_set_index(nnn);
    break;
  case 0xB:
    op_jump_with_offset(x, nn, nnn);
    break;
  case 0xC:
    op_random(x, nn);
    break;
  case 0xD:
    op_draw_sprite(x, y, n);
    should_update_screen = true;
    break;
  case 0xE:
    switch (nn) {
    case 0x9E:
      op_skip_if_key(x);
      break;
    case 0xA1:
      op_skip_if_not_key(x);
      break;
    default:;
    }
    break;
  case 0xF:
    switch (nn) {
    case 0x07:
      op_set_reg_to_delay_timer(x);
      break;
    case 0x15:
      op_set_delay_timer_to_reg(x);
      break;
    case 0x18:
      op_set_sound_timer_to_reg(x);
      break;
    case 0x29:
      op_set_font_char(x);
      break;
    case 0x33:
      op_decode_to_decimal(x);
      break;
    case 0x55:
      op_store_memory(x);
      break;
    case 0x65:
      op_load_memory(x);
      break;
    case 0x0A:
      op_get_key(x);
      break;
    case 0x1E:
      op_add_to_index(x);
      break;
    default:;
    }
    // exit(1);
    break;
  default:
    printf("undefined instruction: %x\n", op_type);
    // exit(1);
  }

  return should_update_screen;
}

void op_jump(uint16_t dst) { program_counter = dst; }

void op_set_register(uint8_t reg, uint8_t value) { v[reg] = value; }

void op_add_to_register(uint8_t reg, uint8_t value) { v[reg] += value; }

void op_set_index(uint16_t value) { index_register = value; }

void op_draw_sprite(uint8_t reg1, uint8_t reg2, uint8_t n) {
  int target_pos_x = v[reg1] & (SCREEN_W - 1);
  int target_pos_y = v[reg2] & (SCREEN_H - 1);
  v[0xf] = 0;

  uint8_t *sprite = memory + index_register;
  for (int i = 0; i < n; ++i) {
    int effective_pos_y = target_pos_y + i;
    if (effective_pos_y >= SCREEN_H) {
      ++sprite;
      continue;
    }

    uint8_t sprite_bits[8];
    byte_to_bits(*sprite, sprite_bits);

    for (int j = 0; j < 8; ++j) {
      int effective_pos_x = target_pos_x + j;
      if (effective_pos_x >= SCREEN_W) {
        break;
      }

      bool current_pixel_state = screen_state[effective_pos_y][effective_pos_x];
      screen_state[effective_pos_y][effective_pos_x] ^= sprite_bits[j];

      if (current_pixel_state &&
          !screen_state[effective_pos_y][effective_pos_x]) {
        v[0xf] = 1;
      }
    }

    ++sprite;
  }
}

void op_clear_screen() {
  for (int i = 0; i < SCREEN_H; ++i) {
    for (int j = 0; j < SCREEN_W; ++j) {
      screen_state[i][j] = 0x0;
    }
  }
}

void op_skip_eq_reg_num(uint8_t reg, uint8_t value) {
  if (v[reg] == value) {
    program_counter += 2;
  }
}
void op_skip_not_eq_reg_num(uint8_t reg, uint8_t value) {
  if (v[reg] != value) {
    program_counter += 2;
  }
}

void op_skip_eq_reg(uint8_t reg1, uint8_t reg2) {
  if (v[reg1] == v[reg2]) {
    program_counter += 2;
  }
}

void op_skip_not_eq_reg(uint8_t reg1, uint8_t reg2) {
  if (v[reg1] != v[reg2]) {
    program_counter += 2;
  }
}

void op_return_subroutine() {
  int16_t next_action = stack_pop(&functions_stack);
  program_counter = next_action;
}

void op_call_subroutine(uint16_t function) {
  stack_push(&functions_stack, program_counter);
  program_counter = function;
}

void op_set(uint8_t reg1, uint8_t reg2) { v[reg1] = v[reg2]; }

void op_binary_or(uint8_t reg1, uint8_t reg2) {
  v[reg1] = v[reg1] | v[reg2];
  v[0xF] = 0;
}

void op_binary_and(uint8_t reg1, uint8_t reg2) {
  v[reg1] = v[reg1] & v[reg2];
  v[0xF] = 0;
}

void op_binary_xor(uint8_t reg1, uint8_t reg2) {
  v[reg1] = v[reg1] ^ v[reg2];
  v[0xF] = 0;
}

void op_add_registers(uint8_t reg1, uint8_t reg2) {
  uint8_t a = v[reg1];
  uint8_t b = v[reg2];
  uint8_t c = a + b;

  v[reg1] = c;
  v[0xF] = c < a ? 1 : 0;
}

void op_vx_minus_vy(uint8_t reg1, uint8_t reg2) {
  uint8_t vx = v[reg1];
  uint8_t vy = v[reg2];
  uint8_t flag = (vx >= vy) ? 1 : 0;

  v[reg1] = vx - vy;
  v[0xF] = flag;
}

void op_vy_minus_vx(uint8_t reg1, uint8_t reg2) {
  uint8_t vx = v[reg1];
  uint8_t vy = v[reg2];
  uint8_t flag = (vy >= vx) ? 1 : 0;

  v[reg1] = vy - vx;
  v[0xF] = flag;
}

void op_shift_right(uint8_t reg1, uint8_t reg2) {
  if (legacy_mode) {
    uint8_t shifted_bit = v[reg2] & 0x01;
    v[reg1] = v[reg2] >> 1;
    v[0xf] = shifted_bit;
    return;
  }

  uint8_t shifted_bit = v[reg1] & 0x01;
  v[reg1] >>= 1;
  v[0xf] = shifted_bit;
}

void op_shift_left(uint8_t reg1, uint8_t reg2) {
  if (legacy_mode) {
    uint8_t shifted_bit = (v[reg2] & 0x80) >> 7;
    v[reg1] = v[reg2] << 1;
    v[0xf] = shifted_bit;
    return;
  }

  uint8_t shifted_bit = (v[reg1] & 0x80) >> 7;
  v[reg1] <<= 1;
  v[0xf] = shifted_bit;
}

void op_jump_with_offset(uint8_t reg1, uint8_t nn, uint16_t nnn) {
  if (legacy_mode) {
    program_counter = nnn;
    program_counter += v[0];
    return;
  }

  program_counter = nnn;
  program_counter += v[reg1];
}

void op_random(uint8_t reg1, uint8_t nn) { v[reg1] = (rand() % 255) & nn; }

void op_skip_if_key(uint8_t reg) {
  uint8_t required_key = v[reg];
  if (keyboard[required_key]) {
    program_counter += 2;
  }
}

void op_skip_if_not_key(uint8_t reg) {
  uint8_t required_key = v[reg];
  if (!keyboard[required_key]) {
    program_counter += 2;
  }
}

void op_set_reg_to_delay_timer(uint8_t reg) { v[reg] = delay_timer; }

void op_set_delay_timer_to_reg(uint8_t reg) { delay_timer = v[reg]; }

void op_set_sound_timer_to_reg(uint8_t reg) { audio_timer = v[reg]; }

void op_add_to_index(uint8_t reg) { index_register += v[reg]; }

void op_get_key(uint8_t reg) {
  // should block until key is pressed
  program_counter -= 2;
}

void op_set_font_char(uint8_t reg) {
  index_register = (v[reg] * 5) + FONT_MEMORY_LOCATION;
}

void op_decode_to_decimal(uint8_t reg) {
  uint8_t val = v[reg];

  memory[index_register + 2] = val % 10;
  val /= 10;

  memory[index_register + 1] = val % 10;
  val /= 10;

  memory[index_register] = val;
}

void op_store_memory(uint8_t reg) {
  for (int i = 0; i <= reg; ++i) {
    memory[index_register + i] = v[i];
  }

  if (legacy_mode) {
    index_register += reg + 1;
  }
}

void op_load_memory(uint8_t reg) {
  for (int i = 0; i <= reg; ++i) {
    v[i] = memory[index_register + i];
  }

  if (legacy_mode) {
    index_register += reg + 1;
  }
}
