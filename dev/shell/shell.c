#include <shell.h>
#include <stdio.h>
#include <keyboard.h>
#include <vga.h>

// External declarations
extern shell_command_t shell_commands[];  // The shell commands array
extern size_t shell_commands_count;        // The number of shell commands

void print_padded(const char *text, int width) {
    int len = strlen(text);
    vga_puts(text);
    for (int i = len; i < width; i++) {
        vga_putchar(' ');
    }
}

// Function to handle the help command
int help_main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    vga_puts("+------------------+-------------------------------------------+\n");
    vga_puts("| Command          | Description                               |\n");
    vga_puts("+------------------+-------------------------------------------+\n");

    for (size_t i = 0; i < shell_commands_count; i++) {
        vga_puts("| ");
        print_padded(shell_commands[i].name, 16);      // Command column
        vga_puts(" | ");
        print_padded(shell_commands[i].description, 41); // Description column
        vga_puts(" |\n");
    }

    vga_puts("+------------------+-------------------------------------------+\n");
    return 0;
}

// Public shell command list (accessible from init.c)
shell_command_t shell_commands[] = {
    { "cat",      "Show the contents of a file",           cat_main      },
    { "clear",    "Clear the screen",                      clear_main    },
    { "cowsay",   "Make a cow say a line",                 cowsay_main   },
    { "cpuinfo",  "Print processor information",           cpuinfo_main  },
    { "echo",     "Display a line of text",                echo_main     },
    { "ed",       "The simple editor",                     ed_main       },
    { "expr",     "Calculate the entered expression",      expr_main     },
    { "factor",   "Display the prime factors of a number", factor_main   },
    { "fetch",    "Fetch OS information",                  fetch_main    },
    { "help",     "Display this help message",             help_main     },
    { "ls",       "List existing files",                   ls_main       },
    { "rm",       "Remove file",                           rm_main       },
    { "rand",     "Generate a random number",              rand_main     },
    { "reboot",   "Reboot the system",                     reboot_main   },
    { "shutdown", "Power off the system",                  shutdown_main },
    { "touch",    "Create a file",                         touch_main    },
    { "tty",      "Print current console session",         tty_main      },
    { "uname",    "Provide OS information",                uname_main    },
    { "whoami",   "Print user information",                whoami_main   },
    { "yes",      "Repeatedly print a string",             yes_main      },
    { "mv",       "Move file/folder",                      mv_main       },
};


// External reference for the command count
size_t shell_commands_count = sizeof(shell_commands) / sizeof(shell_commands[0]);

// Initialize the shell context
void shell_init(shell_context_t *ctx, shell_command_t *commands, size_t num_commands) {
    memset(ctx, 0, sizeof(shell_context_t));
    ctx->commands = commands;
    ctx->num_commands = num_commands;
    ctx->running = true;
    strcpy(ctx->current_path, "/");

    // Set default VGA color
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// Main shell loop
void shell_run(shell_context_t *ctx) {
    while (ctx->running) {
        shell_print_prompt(ctx);

        // Read input character by character
        ctx->input_length = 0;
        while (true) {
            // Read actual keyboard input
            char c = kb_getchar();

            // Handle Enter key press to break the loop
            if (c == '\n') {
                vga_putchar('\n');
                break;
            }

            // Handle backspace
            if (c == '\b' && ctx->input_length > 0) {
                ctx->input_length--;
                vga_puts("\b \b"); // Erase the character
            }
            // Handle ctrl-C (SIGINT) for interruption
            else if (c == 3) { // ASCII for ctrl-C
                vga_puts("\nProcess interrupted.\n");
                ctx->input_length = 0;
                break;
            }
            // Handle regular character input
            else if (c >= 32 && c <= 126 && ctx->input_length < SHELL_MAX_INPUT_LENGTH - 1) {
                ctx->input_buffer[ctx->input_length++] = c;
                vga_putchar(c);
            }
        }

        // Null-terminate the input
        ctx->input_buffer[ctx->input_length] = '\0';

        // Process the input
        shell_process_input(ctx);
    }
}

// Print the shell prompt
void shell_print_prompt(shell_context_t *ctx) {
    vga_puts(ctx->current_path);
    vga_puts(SHELL_PROMPT);
}

// Process the input line
void shell_process_input(shell_context_t *ctx) {
    if (ctx->input_length == 0) {
        return;
    }

    // Tokenize the input
    char *argv[SHELL_MAX_ARGS];
    int argc = 0;

    char *token = strtok(ctx->input_buffer, " ");
    while (token != NULL && argc < SHELL_MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    // Execute the command
    shell_execute(ctx, argc, argv);
}

// Execute a command
int shell_execute(shell_context_t *ctx, int argc, char **argv) {
    if (argc == 0) {
        return 0;
    }

    // Search for the command in registered commands
    for (size_t i = 0; i < ctx->num_commands; i++) {
        if (strcmp(argv[0], ctx->commands[i].name) == 0) {
            return ctx->commands[i].func(argc, argv);
        }
    }

    // Command not found - print error in the desired format
    shell_print_not_found(argv[0]);
    return -1;
}

// Print the "command: not found" error message using vga_puts and vga_putchar
void shell_print_not_found(const char *command) {
    vga_puts(command);
    vga_puts(": not found\n");
}
