#ifndef KEPLAR_TERMINAL_H
#define KEPLAR_TERMINAL_H

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

void terminal_clear(void);
void terminal_putchar(char c);
void terminal_write(const char *string);
void terminal_prompt(void);
void keplar_intro(void);

#endif