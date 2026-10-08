#include <stdint.h>
#include "scroll.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;

void shell_scroll(void) {

    for (uint64_t row = 1; row < VGA_HEIGHT; row++) {
        for (uint64_t column = 0; column < VGA_WIDTH; column++) {

            VGA_MEMORY[
                (row - 1) * VGA_WIDTH + column
            ] =
                VGA_MEMORY[
                    row * VGA_WIDTH + column
                ];
        }
    }

    for (uint64_t column = 0; column < VGA_WIDTH; column++) {
        VGA_MEMORY[
            (VGA_HEIGHT - 1) * VGA_WIDTH + column
        ] = ((uint16_t)0x07 << 8) | ' ';
    }
}
