#include <stdint.h>

#include "terminal.h"

static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;

static uint8_t terminal_color = 0x07;
static uint8_t terminal_row = 0;
static uint8_t terminal_column = 0;

void terminal_clear(void) {
    for (uint64_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint64_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] =
                ((uint16_t)terminal_color << 8) | ' ';
        }
    }

    terminal_row = 0;
    terminal_column = 0;
}

void terminal_putchar(char c) {

    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;

        if (terminal_row >= VGA_HEIGHT)
            terminal_row = 0;

        return;
    }

    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;

            VGA_MEMORY[
                terminal_row * VGA_WIDTH + terminal_column
            ] = ((uint16_t)terminal_color << 8) | ' ';
        }

        return;
    }

    VGA_MEMORY[
        terminal_row * VGA_WIDTH + terminal_column
    ] = ((uint16_t)terminal_color << 8) | (uint8_t)c;

    terminal_column++;

    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

    if (terminal_row >= VGA_HEIGHT)
        terminal_row = 0;
}

void terminal_write(const char *string)
{
    for (uint64_t i = 0; string[i] != '\0'; i++) {
        terminal_putchar(string[i]);
    }
}

void terminal_prompt(void) {
    terminal_write("(kernel@keplar)--[~]\n");
    terminal_write("Keplar:> ");
}

void keplar_intro(void) {
    terminal_write("Hello from Keplar OS v.0.0.0 (Alpha)\n");
    terminal_write("=============================\n");
    terminal_write("Still in it's developing stage!\n");
    terminal_write("=============================\n");
    terminal_write("Welcome to Keplar!\n\n");
}