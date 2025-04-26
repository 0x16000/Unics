#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vga.h>
#include <io.h>
#include <string.h>

#define SHELL_MAX_INPUT_LENGTH 256
#define SHELL_MAX_ARGS 32
#define SHELL_MAX_PATH_LENGTH 128
#define SHELL_PROMPT "unics$ "

// Command structure
typedef struct {
    const char *name;
    const char *description;
    int (*func)(int argc, char **argv);
} shell_command_t;

// Shell context structure
typedef struct {
    char input_buffer[SHELL_MAX_INPUT_LENGTH];
    size_t input_length;
    char current_path[SHELL_MAX_PATH_LENGTH];
    shell_command_t *commands;
    size_t num_commands;
    bool running;
} shell_context_t;

// Shell functions
void shell_init(shell_context_t *ctx, shell_command_t *commands, size_t num_commands);
void shell_run(shell_context_t *ctx);
void shell_print_prompt(shell_context_t *ctx);
void shell_process_input(shell_context_t *ctx);
int shell_execute(shell_context_t *ctx, int argc, char **argv);
void shell_print_error(const char *message);
void shell_print_not_found(const char *command);

// Add external command declaration
extern int clear_main(int argc, char **argv);
extern int echo_main(int argc, char **argv);
extern int yes_main(int argc, char **argv);
extern int expr_main(int argc, char **argv);
extern int uname_main(int argc, char **argv);
extern int shutdown_main(int argc, char **argv);
extern int reboot_main(int argc, char **argv);
extern int cowsay_main(int argc, char **argv);
extern int whoami_main(int argc, char **argv);
extern int rand_main(int argc, char **argv);
extern int factor_main(int argc, char **argv);
extern int tty_main(int argc, char **argv);
extern int cpuinfo_main(int argc, char **argv);
extern int fetch_main(int argc, char **argv);

#endif // SHELL_H
