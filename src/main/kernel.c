#include "terminal.h"
#include "shell.h"
#include "idt.h"
#include "pic.h"

void kernel_main(void)
{
    keplar_intro();
    pic_remap();
    idt_init();


    shell_init();
    shell_run();


}
