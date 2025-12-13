#include "stack.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool stack_is_empty(Stack *s) { return s->head == -1; }
bool stack_is_full(Stack *s) { return s->head == s->max_size; }

void stack_init(Stack *s, int max_size) {
  if (max_size > MAX_ALLOWED_STACK_SIZE) {
    printf("allocating more stack space than allowed is prohibited");
    exit(1);
  }

  s->max_size = max_size;
  s->head = -1;
}

void stack_push(Stack *s, int16_t val) {
  if (stack_is_full(s)) {
    printf("stack overflow!\n");
    exit(1);
  }

  ++s->head;
  s->data[s->head] = val;
}

int16_t stack_pop(Stack *s) {
  if (stack_is_empty(s)) {
    printf("cannot pop empty stack!\n");
    exit(1);
  }

  int val = s->data[s->head];
  --s->head;

  return val;
}

void stack_print(Stack *s) {
  for (int i = 0; i <= s->head; ++i) {
    printf("%d ", s->data[i]);
  }

  printf("\n");
}

int16_t stack_peek(Stack *s) {
  if (stack_is_empty(s)) {
    printf("cannot peek empty stack\n");
    exit(1);
  }

  return s->data[s->head];
}
