#include <stdint.h>

#include "terminal.h"

void status_function(uint8_t staus) {
    if (staus) {
        terminal_write("[   ");
        cterminal_write("OK", 0x02);
        terminal_write("   ]");
    }
    else
    {
        terminal_write("[ ");
        cterminal_write("Failed", 0x04);
        terminal_write(" ]");
    }
}
