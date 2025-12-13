#ifndef STACK_H
#define STACK_H

#define MAX_ALLOWED_STACK_SIZE 256

#include <stdbool.h>
#include <stdint.h>

typedef struct stack {
  int head;
  int max_size;
  int16_t data[MAX_ALLOWED_STACK_SIZE];
} Stack;

void stack_print(Stack *s);
int16_t stack_pop(Stack *s);
int16_t stack_peek(Stack *s);
bool stack_is_full(Stack *s);
bool stack_is_empty(Stack *s);
void stack_init(Stack *s, int max_size);
void stack_push(Stack *s, int16_t val);

#endif // !STACK_H
