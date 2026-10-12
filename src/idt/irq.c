#include <stdint.h>

#include "io.h"
#include "pic.h"
#include "keyboard.h"

volatile uint64_t timer_ticks = 0;

void irq0_handler(void)
{
    timer_ticks++;
    pic_send_eoi(0);
}

void irq1_handler(void)
{
    uint8_t status = inb(0x64);

    if (status & 0x01) {
        uint8_t scancode = inb(0x60);
        keyboard_handle_scancode(scancode);
    }

    pic_send_eoi(1);
}