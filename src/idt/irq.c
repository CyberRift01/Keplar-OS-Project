
#include <stdint.h>

#include "io.h"
#include "pic.h"
#include "keyboard.h"

volatile uint64_t timer_ticks = 0;

void irq0_handler(void)
{
    timer_ticks++;
}

void irq1_handler(void)
{
    uint8_t status = inb(0x64);

    if (status & 0x01) {
        uint8_t scancode = inb(0x60);
        keyboard_handle_scancode(scancode);
    }
}

void irq_dispatch_handler(uint64_t irq_number)
{
    if (irq_number >= 16)
        return;

    uint8_t irq = (uint8_t)irq_number;

    /*
     * Spurious IRQ7:
     * The master PIC did not set IRQ7's in-service bit.
     * Do not send an EOI.
     */
    if (irq == 7 && pic_is_spurious_irq(7))
        return;

    /*
     * Spurious IRQ15:
     * The slave PIC did not set IRQ15's in-service bit.
     * Acknowledge only the master PIC's cascade IRQ2.
     */
    if (irq == 15 && pic_is_spurious_irq(15)) {
        pic_send_eoi(2);
        return;
    }

    /*
     * Dispatch device-specific handlers.
     * Other IRQs are acknowledged even when no handler
     * has been implemented yet.
     */
    switch (irq) {
        case 0:
            irq0_handler();
            break;

        case 1:
            irq1_handler();
            break;

        default:
            break;
    }

    /* Centralized PIC acknowledgement. */
    pic_send_eoi(irq);
}