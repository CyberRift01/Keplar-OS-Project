#include <stdint.h>

#include "terminal.h"
#include "status.h"
#include "timer.h"


/*
 * UTILITY
*/

#include <stdint.h>

uint64_t kernel_strlen(const char *str)
{
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    
    return length;
}

uint64_t keplar_atoi(const char *str)
{
    uint64_t result = 0;

    if (str == 0 || str[0] == '\0')
        return 0;

    for (uint64_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9')
            return 0;

        uint64_t digit = (uint64_t)(str[i] - '0');

        if (result > (UINT64_MAX - digit) / 10)
            return 0;

        result = result * 10 + digit;
    }

    return result;
}

uint8_t keplar_cmp_str(const char *str1, const char *str2)
{
    uint64_t len= kernel_strlen(str2);
    if (len!=kernel_strlen(str1))
        return 0;
    else
    {
        for (uint64_t i=0; i<len; i++)
        {
            if (str1[i]!=str2[i])
                return 0;
        }
    }
    return 1;
}

/*
 * END OF UTILITY
*/

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
    terminal_write("Welcome to Keplar!\n");
    terminal_write("Keplar is going to boot: ");
    status_function(1);
    terminal_write("\n\n");
    terminal_write("Hello from Keplar OS v.0.0.0 (Alpha)\n");
    terminal_write("=============================\n");
    terminal_write("Still in it's developing stage!\n");
    terminal_write("=============================\n");
    terminal_write("Welcome to Keplar!\n\n");
}

void cterminal_putchar(char c, uint8_t color) {

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
            ] = ((uint16_t)color << 8) | ' ';
        }

        return;
    }

    VGA_MEMORY[
        terminal_row * VGA_WIDTH + terminal_column
    ] = ((uint16_t)color << 8) | (uint8_t)c;

    terminal_column++;

    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

    if (terminal_row >= VGA_HEIGHT)
        terminal_row = 0;
}


void cterminal_write(const char *string, uint8_t color)
{
    for (uint64_t i = 0; string[i] != '\0'; i++) {
        cterminal_putchar(string[i], color);
    }
}

void terminal_write_uint(uint64_t value)
{
    char digits[20];
    unsigned int i = 0;

    if (value == 0) {
        terminal_putchar('0');
        return;
    }

    while (value > 0) {
        digits[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        terminal_putchar(digits[--i]);
    }
}

uint8_t terminal_match_char_flag(const char *string, const char *flag)
{
    
    if (!keplar_cmp_str(string,flag)){
        cterminal_write("Invalid flag!", 0x04);
        return 0;
    }
    else
        return 1;
}
