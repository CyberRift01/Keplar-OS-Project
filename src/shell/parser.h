#ifndef KEPLAR_PARSER_H
#define KEPLAR_PARSER_H

#define SHELL_MAX_ARGS 16

typedef struct {
    int argc;
    char *argv[SHELL_MAX_ARGS];
} shell_command_t;

typedef enum {
    SHELL_PARSE_OK = 0,
    SHELL_PARSE_TOO_MANY_ARGS,
    SHELL_PARSE_UNTERMINATED_QUOTE,
    SHELL_PARSE_TRAILING_ESCAPE
} shell_parse_result_t;

shell_parse_result_t shell_parse(
    char *input,
    shell_command_t *command
);

#endif