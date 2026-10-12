#include <stdint.h>

#include "idt.h"
#include "terminal.h"
#include "status.h"

#define IDT_ENTRIES 256

extern void exception_handler(void);
extern void irq0_stub(void);
extern void irq1_stub(void);

/*
 * IRQ0 to IRQ15
*/

extern void irq0_stub(void);
extern void irq1_stub(void);
extern void irq2_stub(void);
extern void irq3_stub(void);
extern void irq4_stub(void);
extern void irq5_stub(void);
extern void irq6_stub(void);
extern void irq7_stub(void);
extern void irq8_stub(void);
extern void irq9_stub(void);
extern void irq10_stub(void);
extern void irq11_stub(void);
extern void irq12_stub(void);
extern void irq13_stub(void);
extern void irq14_stub(void);
extern void irq15_stub(void);

/*
 * End Of IQRs
*/

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idt_ptr;

static void idt_set_gate(
    uint16_t vector,
    uint64_t handler,
    uint16_t selector,
    uint8_t type_attr
) {
    idt[vector].offset_low =
        (uint16_t)(handler & 0xFFFF);

    idt[vector].selector = selector;
    idt[vector].ist = 0;
    idt[vector].type_attr = type_attr;

    idt[vector].offset_mid =
        (uint16_t)((handler >> 16) & 0xFFFF);

    idt[vector].offset_high =
        (uint32_t)((handler >> 32) & 0xFFFFFFFF);

    idt[vector].zero = 0;
}

void idt_init(void)
{
    __asm__ volatile ("cli" ::: "memory");

    for (uint16_t i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(
            i,
            (uint64_t)exception_handler,
            0x08,
            0x8E
        );
    }

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)idt;

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_ptr)
        : "memory"
    );
    terminal_write("Enabling IDT:            ");
    status_function(1);
    terminal_putchar('\n');
}

void idt_set_irq_gates(void)
{
    idt_set_gate(
        0x20,
        (uint64_t)irq0_stub,
        0x08,
        0x8E
    );

    idt_set_gate(
        0x21,
        (uint64_t)irq1_stub,
        0x08,
        0x8E
    );
}