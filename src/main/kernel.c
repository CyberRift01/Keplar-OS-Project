#include "terminal.h"
#include "shell.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"

void kernel_main(void) {
    shell_init();
    shell_run();
    
}
