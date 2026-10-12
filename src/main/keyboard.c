#include <stdint.h>

#include "io.h"
#include "keyboard.h"

#define KEY_COUNT 128
#define QUEUE_SIZE 128
#define QUEUE_MASK (QUEUE_SIZE - 1)

// cancel_flag
static volatile uint8_t cancel_requested;

/* PS/2 Set 1: unshifted and shifted with caps characters. */
static const char normal_map[KEY_COUNT] = {
    [0x01] = 27,
    [0x02] = '1', [0x03] = '2', [0x04] = '3',
    [0x05] = '4', [0x06] = '5', [0x07] = '6',
    [0x08] = '7', [0x09] = '8', [0x0A] = '9',
    [0x0B] = '0', [0x0C] = '-', [0x0D] = '=',
    [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e',
    [0x13] = 'r', [0x14] = 't', [0x15] = 'y',
    [0x16] = 'u', [0x17] = 'i', [0x18] = 'o',
    [0x19] = 'p', [0x1A] = '[', [0x1B] = ']',
    [0x1C] = '\n',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd',
    [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x27] = ';', [0x28] = '\'',
    [0x29] = '`', [0x2B] = '\\',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c',
    [0x2F] = 'v', [0x30] = 'b', [0x31] = 'n',
    [0x32] = 'm', [0x33] = ',', [0x34] = '.',
    [0x35] = '/', [0x37] = '*', [0x39] = ' ',
    [0x4A] = '-', [0x4E] = '+',
};

static const char shifted_map[KEY_COUNT] = {
    [0x01] = 27,
    [0x02] = '!', [0x03] = '@', [0x04] = '#',
    [0x05] = '$', [0x06] = '%', [0x07] = '^',
    [0x08] = '&', [0x09] = '*', [0x0A] = '(',
    [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
    [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'Q', [0x11] = 'W', [0x12] = 'E',
    [0x13] = 'R', [0x14] = 'T', [0x15] = 'Y',
    [0x16] = 'U', [0x17] = 'I', [0x18] = 'O',
    [0x19] = 'P', [0x1A] = '{', [0x1B] = '}',
    [0x1C] = '\n',
    [0x1E] = 'A', [0x1F] = 'S', [0x20] = 'D',
    [0x21] = 'F', [0x22] = 'G', [0x23] = 'H',
    [0x24] = 'J', [0x25] = 'K', [0x26] = 'L',
    [0x27] = ':', [0x28] = '"',
    [0x29] = '~', [0x2B] = '|',
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C',
    [0x2F] = 'V', [0x30] = 'B', [0x31] = 'N',
    [0x32] = 'M', [0x33] = '<', [0x34] = '>',
    [0x35] = '?', [0x37] = '*', [0x39] = ' ',
    [0x4A] = '-', [0x4E] = '+',
};

/* down[0] = ordinary keys; down[1] = E0-prefixed keys. */
static volatile uint8_t down[2][KEY_COUNT];

static volatile uint8_t left_shift;
static volatile uint8_t right_shift;
static volatile uint8_t left_ctrl;
static volatile uint8_t right_ctrl;
static volatile uint8_t left_alt;
static volatile uint8_t right_alt;
static volatile uint8_t caps_lock;
static uint8_t e0_prefix;

/* Single-producer (IRQ), single-consumer (shell) ring buffer. */
static char char_queue[QUEUE_SIZE];
static volatile uint8_t queue_head;
static volatile uint8_t queue_tail;

static void queue_character(char c)
{
    uint8_t head = queue_head;
    uint8_t next = (uint8_t)((head + 1) & QUEUE_MASK);

    if (c == 3) {
    cancel_requested = 1;
    return;
    }

    /* Leave one slot empty to distinguish full from empty. */
    if (next == queue_tail)
        return; /* Full: drop this character. */

    char_queue[head] = c;
    queue_head = next;
}


void keyboard_clear_cancel(void)
{
    cancel_requested = 0;
}

int keyboard_take_cancel(void)
{
    if (!cancel_requested)
        return 0;

    cancel_requested = 0;
    return 1;
}

void keyboard_init(void)
{
    cancel_requested = 0;
    for (unsigned int i = 0; i < 2; i++) {
        for (unsigned int j = 0; j < KEY_COUNT; j++)
            down[i][j] = 0;
    }

    left_shift = right_shift = 0;
    left_ctrl = right_ctrl = 0;
    left_alt = right_alt = 0;
    caps_lock = 0;
    e0_prefix = 0;
    queue_head = queue_tail = 0;
}

int keyboard_is_key_down(uint8_t scancode, int extended)
{
    if (scancode >= KEY_COUNT)
        return 0;

    return down[extended ? 1 : 0][scancode] != 0;
}

int keyboard_readchar(char *out)
{
    if (out == 0)
        return 0;

    uint8_t tail = queue_tail;

    if (tail == queue_head)
        return 0;

    *out = char_queue[tail];
    queue_tail = (uint8_t)((tail + 1) & QUEUE_MASK);
    return 1;
}

void keyboard_handle_scancode(uint8_t scancode)
{
    /* E0 marks the next byte as an extended scancode. */
    if (scancode == 0xE0) {
        e0_prefix = 1;
        return;
    }

    /* E1 starts the special Pause-key sequence.
       This basic driver ignores that sequence. */
    if (scancode == 0xE1) {
        e0_prefix = 0;
        return;
    }

    uint8_t extended = e0_prefix;
    e0_prefix = 0;

    uint8_t released = (scancode & 0x80) != 0;
    uint8_t code = scancode & 0x7F;

    /* Print Screen has a special multi-byte sequence. */
    if (extended && (code == 0x2A || code == 0x36))
        return;

    if (code >= KEY_COUNT)
        return;

    uint8_t was_down = down[extended][code];

    down[extended][code] = released ? 0 : 1;

    /* Update modifier states on both press and release. */
    if (!extended && code == 0x2A) {
        left_shift = !released;
        return;
    }
    if (!extended && code == 0x36) {
        right_shift = !released;
        return;
    }
    if (code == 0x1D) {
        if (extended)
            right_ctrl = !released;
        else
            left_ctrl = !released;
        return;
    }
    if (code == 0x38) {
        if (extended)
            right_alt = !released;
        else
            left_alt = !released;
        return;
    }

    /* Toggle Caps Lock only on a new press, not on release. */
    if (!extended && code == 0x3A) {
        if (!released && !was_down)
            caps_lock = !caps_lock;
        return;
    }

    /* Break events update state but never generate characters. */
    if (released || extended)
        return;

    char c = normal_map[code];
    if (c == 0)
        return;

    uint8_t shift = left_shift || right_shift;
    uint8_t ctrl = left_ctrl || right_ctrl;
    uint8_t alt = left_alt || right_alt;

    (void)alt; /* Tracked for future Alt shortcuts. */

    /* Caps Lock affects letters, not punctuation. */
    if (c >= 'a' && c <= 'z') {
        if (shift != caps_lock)
            c = (char)(c - 'a' + 'A');
    } else if (shift) {
        c = shifted_map[code];
    }

    /* Ctrl+A through Ctrl+Z become ASCII control codes 1–26. */
    if (ctrl && c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 1);
    else if (ctrl && c >= 'A' && c <= 'Z')
        c = (char)(c - 'A' + 1);

    /* A NUL byte cannot be represented in this character queue. */
    if (c != 0)
        queue_character(c);
}