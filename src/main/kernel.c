#include "terminal.h"
#include "shell.h"
#include "idt.h"

void kernel_main(void) {
    idt_init();
    shell_init();
    shell_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
