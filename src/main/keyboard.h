
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);

/* Feed one raw PS/2 Set 1 scancode to the decoder. */
void keyboard_handle_scancode(uint8_t scancode);

/* Returns 1 if a character was read; otherwise returns 0. */
int keyboard_readchar(char *out);

/* Query whether a physical key is currently held.
   extended = 0 for ordinary keys, 1 for E0-prefixed keys. */
int keyboard_is_key_down(uint8_t scancode, int extended);
void keyboard_clear_cancel(void);
int keyboard_take_cancel(void);

#endif