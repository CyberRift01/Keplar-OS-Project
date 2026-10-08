#include <stdint.h>
#include "io.h"
#include "keyboard.h"

static const char keyboard_map[128] = {
    0,
    27,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=',
    '\b',
    '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',
    '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'',
    '`',
    0,
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/',
    0,
    '*',
    0,
    ' ',
};

char keyboard_getchar(void)
{
    uint8_t status = inb(0x64);

    if (!(status & 0x01))
        return 0;

    uint8_t scancode = inb(0x60);

    if (scancode & 0x80)
        return 0;

    if (scancode >= 128)
        return 0;

    return keyboard_map[scancode];
}