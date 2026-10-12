#include <stdint.h>

#include "io.h"
#include "pic.h"
#include "timer.h"
#include "terminal.h"
#include "status.h"
#include "keyboard.h"

extern volatile uint64_t timer_ticks;

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_BASE_FREQUENCY 1193182u
#define PIT_FREQUENCY 100u

void timer_init(void)
{
    uint16_t divisor =
        (uint16_t)(PIT_BASE_FREQUENCY / PIT_FREQUENCY);

    /* Channel 0, low byte then high byte, square-wave mode. */
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)(divisor >> 8));

    /* Allow timer IRQ0 through the PIC. */
    pic_unmask_irq(0);
    terminal_write("Enabling Timer:          ");
    status_function(1);
    terminal_putchar('\n');
}

uint64_t timer_get_ticks(void)
{
    return timer_ticks;
}

uint8_t timer_sleep(uint64_t seconds)
{
    uint64_t start = timer_get_ticks();
    uint64_t duration = seconds * 100;

    while ((timer_get_ticks() - start) < duration) {
        if (keyboard_take_cancel())
            return 1;
        __asm__ volatile ("pause");
    }
    return 0;
}

uint8_t ctimer_sleep(uint64_t seconds)
{
    uint64_t start = timer_get_ticks();

    for (uint64_t i = 1; i <= seconds; i++) {
        uint64_t target = i * 100;

        while ((timer_get_ticks() - start) < target) {
            if (keyboard_take_cancel())
                return 1;
            __asm__ volatile ("pause");
        }

        terminal_write("Sleeping for ");

        terminal_write_uint(i);
        terminal_write(i == 1 ? " second...\n" : " seconds...\n");
    }

    terminal_write("Keplar back on business!\n");
    return 0;
}