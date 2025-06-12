#include <shell.h>
#include <stdio.h>
#include <keyboard.h>
#include <vga.h>
#include <limits.h>

#define CMD_COL_WIDTH 18
#define DESC_COL_WIDTH 45

// Global shell context for built-in commands
static shell_context_t *g_shell_ctx = NULL;

// External declarations
extern shell_command_t shell_commands[];
extern size_t shell_commands_count;

// Utility function for aligned printing
void print_padded(const char *text, int width) {
    int len = strlen(text);
    vga_puts(text);
    for (int i = len; i < width; i++) {
        vga_putchar(' ');
    }
}

int help_main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Paging configuration
    const size_t COMMANDS_PER_PAGE = 10;
    size_t current_page = 0;
    size_t total_pages = (shell_commands_count + COMMANDS_PER_PAGE - 1) / COMMANDS_PER_PAGE;
    bool show_paging = shell_commands_count > COMMANDS_PER_PAGE;

    while (true) {
        vga_puts("Available commands:\n\n");

        // Top border
        vga_putchar('+');
        for (int i = 0; i < CMD_COL_WIDTH; i++) vga_putchar('-');
        vga_putchar('+');
        for (int i = 0; i < DESC_COL_WIDTH; i++) vga_putchar('-');
        vga_puts("+\n");

        // Header
        vga_puts("| ");
        print_padded("Command", CMD_COL_WIDTH - 2);
        vga_puts(" | ");
        print_padded("Description", DESC_COL_WIDTH - 2);
        vga_puts(" |\n");

        // Separator
        vga_putchar('+');
        for (int i = 0; i < CMD_COL_WIDTH; i++) vga_putchar('-');
        vga_putchar('+');
        for (int i = 0; i < DESC_COL_WIDTH; i++) vga_putchar('-');
        vga_puts("+\n");

        // Command entries for current page
        size_t start_idx = current_page * COMMANDS_PER_PAGE;
        size_t end_idx = start_idx + COMMANDS_PER_PAGE;
        if (end_idx > shell_commands_count) end_idx = shell_commands_count;

        for (size_t i = start_idx; i < end_idx; i++) {
            vga_puts("| ");
            print_padded(shell_commands[i].name, CMD_COL_WIDTH - 2);
            vga_puts(" | ");
            print_padded(shell_commands[i].description, DESC_COL_WIDTH - 2);
            vga_puts(" |\n");
        }

        // Bottom border
        vga_putchar('+');
        for (int i = 0; i < CMD_COL_WIDTH; i++) vga_putchar('-');
        vga_putchar('+');
        for (int i = 0; i < DESC_COL_WIDTH; i++) vga_putchar('-');
        vga_puts("+\n");

        // Page navigation info
        if (show_paging) {
            vga_puts("\nPage ");
            // Simple number printing without snprintf
            size_t page_num = current_page + 1;
            if (page_num >= 10) vga_putchar('0' + (page_num / 10));
            vga_putchar('0' + (page_num % 10));
            vga_puts("/");
            if (total_pages >= 10) vga_putchar('0' + (total_pages / 10));
            vga_putchar('0' + (total_pages % 10));
            vga_puts(" - [N]ext, [P]revious, [Q]uit\n");
        } else {
            vga_puts("\nPress any key to continue...\n");
        }

        // Get user input for navigation
        char input = kb_getchar();
        vga_putchar('\n'); // Move to new line after input

        if (!show_paging) {
            break; // Exit after one page if no paging needed
        }

        // Simple lowercase conversion without tolower()
        if (input >= 'A' && input <= 'Z') {
            input += ('a' - 'A');
        }

        switch (input) {
            case 'n': // Next page
                if (current_page < total_pages - 1) {
                    current_page++;
                } else {
                    vga_puts("Already on last page.\n");
                }
                break;
            case 'p': // Previous page
                if (current_page > 0) {
                    current_page--;
                } else {
                    vga_puts("Already on first page.\n");
                }
                break;
            case 'q': // Quit
                return 0;
            default:
                vga_puts("Invalid option. Use N/P/Q for navigation.\n");
                break;
        }

        // Clear screen for next page
        vga_puts("\n");
    }

    return 0;
}

// History command implementation
int history_main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (!g_shell_ctx) {
        vga_puts("History not available\n");
        return 1;
    }

    vga_puts("Command history:\n");
    for (int i = 0; i < g_shell_ctx->history_count; i++) {
        vga_puts("  ");
        char num_str[8];
        // Simple integer to string conversion
        int num = i + 1;
        int digits = 0;
        int temp = num;
        while (temp > 0) {
            temp /= 10;
            digits++;
        }
        for (int j = digits - 1; j >= 0; j--) {
            num_str[j] = '0' + (num % 10);
            num /= 10;
        }
        num_str[digits] = '\0';
        
        vga_puts(num_str);
        vga_puts("  ");
        vga_puts(g_shell_ctx->history[i]);
        vga_putchar('\n');
    }
    return 0;
}

// Exit command implementation
int exit_main(int argc, char **argv) {
    int exit_code = 0;
    
    if (argc > 1) {
        // Simple string to integer conversion
        exit_code = 0;
        char *str = argv[1];
        bool negative = false;
        
        if (*str == '-') {
            negative = true;
            str++;
        }
        
        while (*str >= '0' && *str <= '9') {
            exit_code = exit_code * 10 + (*str - '0');
            str++;
        }
        
        if (negative) exit_code = -exit_code;
    }
    
    vga_puts("Goodbye!\n");
    if (g_shell_ctx) {
        g_shell_ctx->running = false;
    }
    return exit_code;
}

// Enhanced shell command list
shell_command_t shell_commands[] = {
    { "bc",       "Launch a basic calculator",                 bc_main       },
    { "cat",      "Display the contents of a file",            cat_main      },
    { "cd",       "Change the current directory",              cd_main       },
    { "cp",       "Copy a file to a destination",              cp_main       },
    { "clear",    "Clear the terminal screen",                 clear_main    },
    { "cowsay",   "Display a message from a talking cow",      cowsay_main   },
    { "cpuinfo",  "Show processor information",                cpuinfo_main  },
    { "echo",     "Print a line of text",                      echo_main     },
    { "ed",       "Launch a simple text editor",               ed_main       },
    { "exit",     "Exit the shell",                            exit_main     },
    { "expr",     "Evaluate an arithmetic expression",         expr_main     },
    { "factor",   "Show the prime factors of a number",        factor_main   },
    { "fetch",    "Display system information",                fetch_main    },
    { "help",     "Show this help message",                    help_main     },
    { "history",  "Show command history",                      history_main  },
    { "ls",       "List files in the current directory",       ls_main       },
    { "mkdir",    "Create a new directory",                    mkdir_main    },
    { "mv",       "Move or rename a file or directory",        mv_main       },
    { "ps",       "List running processes",                    ps_main       },
    { "pwd",      "Show the current working directory",        pwd_main      },
    { "rand",     "Generate a random number",                  rand_main     },
    { "reboot",   "Reboot the system",                         reboot_main   },
    { "rm",       "Remove a file",                             rm_main       },
    { "rmdir",    "Remove an empty directory",                 rmdir_main    },
    { "shutdown", "Shut down the system",                      shutdown_main },
    { "touch",    "Create an empty file",                      touch_main    },
    { "tty",      "Show the current terminal",                 tty_main      },
    { "uname",    "Show system name and version",              uname_main    },
    { "whoami",   "Display the current user",                  whoami_main   },
    { "yes",      "Repeat a string endlessly",                 yes_main      },
};

size_t shell_commands_count = sizeof(shell_commands) / sizeof(shell_commands[0]);

// Initialize the shell context
void shell_init(shell_context_t *ctx, shell_command_t *commands, size_t num_commands) {
    memset(ctx, 0, sizeof(shell_context_t));
    ctx->commands = commands;
    ctx->num_commands = num_commands;
    ctx->running = true;
    ctx->cursor_pos = 0;
    ctx->insert_mode = true;
    ctx->history_count = 0;
    ctx->history_index = -1;
    ctx->last_exit_status = 0;
    
    // Set global context for built-in commands
    g_shell_ctx = ctx;

    // Set default VGA color and print welcome message
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Type 'help' for available commands, 'exit' to quit.\n\n");
}

// Add command to history
void shell_add_to_history(shell_context_t *ctx, const char *command) {
    if (strlen(command) == 0) return;
    
    // Don't add duplicate consecutive commands
    if (ctx->history_count > 0 && 
        strcmp(ctx->history[ctx->history_count - 1], command) == 0) {
        return;
    }
    
    // Shift history if full
    if (ctx->history_count >= SHELL_HISTORY_SIZE) {
        for (int i = 0; i < SHELL_HISTORY_SIZE - 1; i++) {
            strcpy(ctx->history[i], ctx->history[i + 1]);
        }
        ctx->history_count = SHELL_HISTORY_SIZE - 1;
    }
    
    // Add new command
    strcpy(ctx->history[ctx->history_count], command);
    ctx->history_count++;
}

// Navigate command history
void shell_navigate_history(shell_context_t *ctx, int direction) {
    if (ctx->history_count == 0) return;
    
    if (direction > 0) { // Up arrow
        if (ctx->history_index == -1) {
            ctx->history_index = ctx->history_count - 1;
        } else if (ctx->history_index > 0) {
            ctx->history_index--;
        }
    } else { // Down arrow
        if (ctx->history_index != -1) {
            ctx->history_index++;
            if (ctx->history_index >= ctx->history_count) {
                ctx->history_index = -1;
            }
        }
    }
    
    // Clear current input and load history
    shell_clear_input_line(ctx);
    
    if (ctx->history_index != -1) {
        strcpy(ctx->input_buffer, ctx->history[ctx->history_index]);
        ctx->input_length = strlen(ctx->input_buffer);
    } else {
        ctx->input_buffer[0] = '\0';
        ctx->input_length = 0;
    }
    
    ctx->cursor_pos = ctx->input_length;
    shell_redraw_input(ctx);
}

// Clear input line on screen
void shell_clear_input_line(shell_context_t *ctx) {
    // Move cursor to beginning of input
    for (size_t i = 0; i < ctx->cursor_pos; i++) {
        vga_putchar('\b');
    }
    
    // Clear the line
    for (size_t i = 0; i < ctx->input_length; i++) {
        vga_putchar(' ');
    }
    
    // Move cursor back to beginning
    for (size_t i = 0; i < ctx->input_length; i++) {
        vga_putchar('\b');
    }
}

// Redraw input line
void shell_redraw_input(shell_context_t *ctx) {
    for (size_t i = 0; i < ctx->input_length; i++) {
        vga_putchar(ctx->input_buffer[i]);
    }
    
    // Position cursor correctly
    for (size_t i = ctx->cursor_pos; i < ctx->input_length; i++) {
        vga_putchar('\b');
    }
}

// Enhanced main shell loop with improved input handling
void shell_run(shell_context_t *ctx) {
    while (ctx->running) {
        shell_print_prompt(ctx);

        // Reset input state
        ctx->input_length = 0;
        ctx->cursor_pos = 0;
        ctx->history_index = -1;
        
        while (true) {
            char c = kb_getchar();

            // Handle Enter key
            if (c == '\n') {
                vga_putchar('\n');
                break;
            }
            
            // Handle backspace
            else if (c == '\b' && ctx->cursor_pos > 0) {
                // Move characters left
                for (size_t i = ctx->cursor_pos - 1; i < ctx->input_length - 1; i++) {
                    ctx->input_buffer[i] = ctx->input_buffer[i + 1];
                }
                
                ctx->input_length--;
                ctx->cursor_pos--;
                
                // Redraw line
                vga_putchar('\b');
                for (size_t i = ctx->cursor_pos; i < ctx->input_length; i++) {
                    vga_putchar(ctx->input_buffer[i]);
                }
                vga_putchar(' ');
                vga_putchar('\b');
                
                // Position cursor
                for (size_t i = ctx->cursor_pos; i < ctx->input_length; i++) {
                    vga_putchar('\b');
                }
            }
            
            // Handle Ctrl+C (interrupt)
            else if (c == 3) {
                vga_puts("^C\n");
                ctx->input_length = 0;
                break;
            }
            
            // Handle arrow keys (simplified - assumes ANSI escape sequences)
            else if (c == 27) { // ESC
                char next1 = kb_getchar();
                char next2 = kb_getchar();
                if (next1 == '[') {
                    if (next2 == 'A') { // Up arrow
                        shell_navigate_history(ctx, 1);
                    } else if (next2 == 'B') { // Down arrow
                        shell_navigate_history(ctx, -1);
                    }
                    // Note: Left/Right arrow support could be added here
                }
            }
            
            // Handle regular character input
            else if (c >= 32 && c <= 126 && ctx->input_length < SHELL_MAX_INPUT_LENGTH - 1) {
                // Insert character at cursor position
                for (size_t i = ctx->input_length; i > ctx->cursor_pos; i--) {
                    ctx->input_buffer[i] = ctx->input_buffer[i - 1];
                }
                
                ctx->input_buffer[ctx->cursor_pos] = c;
                ctx->input_length++;
                ctx->cursor_pos++;
                
                // Redraw from cursor position
                for (size_t i = ctx->cursor_pos - 1; i < ctx->input_length; i++) {
                    vga_putchar(ctx->input_buffer[i]);
                }
                
                // Position cursor correctly
                for (size_t i = ctx->cursor_pos; i < ctx->input_length; i++) {
                    vga_putchar('\b');
                }
            }
        }

        // Null-terminate and process input
        ctx->input_buffer[ctx->input_length] = '\0';
        
        if (ctx->input_length > 0) {
            shell_add_to_history(ctx, ctx->input_buffer);
            shell_process_input(ctx);
        }
    }
}

// Enhanced prompt with exit status indicator
void shell_print_prompt(shell_context_t *ctx) {
    // Show exit status of last command if non-zero
    if (ctx->last_exit_status != 0) {
        vga_puts("[");
        // Simple integer to string for exit status
        if (ctx->last_exit_status < 0) {
            vga_putchar('-');
            ctx->last_exit_status = -ctx->last_exit_status;
        }
        
        char status_str[12];
        int digits = 0;
        int temp = ctx->last_exit_status;
        if (temp == 0) {
            status_str[0] = '0';
            digits = 1;
        } else {
            while (temp > 0) {
                status_str[digits++] = '0' + (temp % 10);
                temp /= 10;
            }
        }
        
        // Reverse the string
        for (int i = 0; i < digits / 2; i++) {
            char tmp = status_str[i];
            status_str[i] = status_str[digits - 1 - i];
            status_str[digits - 1 - i] = tmp;
        }
        status_str[digits] = '\0';
        
        vga_puts(status_str);
        vga_puts("] ");
    }

    vga_puts("root@unics:");
    vga_puts(cwd);
    vga_puts(" # ");
}

// Process input with improved error handling
void shell_process_input(shell_context_t *ctx) {
    if (ctx->input_length == 0) {
        return;
    }

    // Tokenize input
    char *argv[SHELL_MAX_ARGS];
    int argc = 0;

    char *token = strtok(ctx->input_buffer, " \t");
    while (token != NULL && argc < SHELL_MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;

    if (argc > 0) {
        ctx->last_exit_status = shell_execute(ctx, argc, argv);
    }
}

// Execute command with better error reporting
int shell_execute(shell_context_t *ctx, int argc, char **argv) {
    if (argc == 0) {
        return 0;
    }

    // Search for command
    for (size_t i = 0; i < ctx->num_commands; i++) {
        if (strcmp(argv[0], ctx->commands[i].name) == 0) {
            return ctx->commands[i].func(argc, argv);
        }
    }

    // Command not found
    shell_print_not_found(argv[0]);
    return 127; // Standard "command not found" exit code
}

// Enhanced error message
void shell_print_not_found(const char *command) {
    vga_puts("shell: ");
    vga_puts(command);
    vga_puts(": command not found\n");
}
