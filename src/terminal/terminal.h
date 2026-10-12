#ifndef KEPLAR_TERMINAL_H
#define KEPLAR_TERMINAL_H

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

void terminal_clear(void);
void terminal_putchar(char c);
void terminal_write(const char *string);
void terminal_prompt(void);
void keplar_intro(void);
void cterminal_putchar(char c, uint8_t color);
void cterminal_write(const char *string, uint8_t color);
void terminal_write_uint(uint64_t value);
uint8_t terminal_match_char_flag(const char *string, const char *flag);
uint64_t kernel_strlen(const char *str);
uint64_t keplar_atoi(const char *str);
uint8_t keplar_cmp_str(const char *str1, const char *str2);
void terminal_backward_put_char(char c, uint8_t column);
void cterminal_backward_put_char(char c, uint8_t column, uint8_t color);
void terminal_backward_write(const char *string, uint8_t column);
void cterminal_backward_write(const char *string, uint8_t column, uint8_t color);

#endif