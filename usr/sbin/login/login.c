#include <login.h>
#include <string.h>
#include <vga.h>
#include <keyboard.h>
#include <stdio.h>
#include <vers.h>

static void clear_input_buffer(char* buffer, size_t size) {
    memset(buffer, 0, size);
}

char* login_get_username(char* buffer, size_t size) {
    vga_puts("login: ");
    vga_enable_cursor();
    size_t pos = 0;
    clear_input_buffer(buffer, size);

    while (true) {
        char c = kb_getchar();
        if (c == '\n') {
            buffer[pos] = '\0';
            vga_putchar('\n'); // Add a newline after the username
            return buffer;
        } else if (c == '\b' && pos > 0) {
            pos--;
            vga_puts("\b \b");
        } else if (pos < size - 1 && c >= 32 && c <= 126) {
            buffer[pos++] = c;
            vga_putchar(c);
        }
    }
}

char* login_get_password(char* buffer, size_t size) {
    printf("password: ");
    vga_disable_cursor(); // Disable cursor at the start of password input
    size_t pos = 0;
    clear_input_buffer(buffer, size);

    while (true) {
        char c = kb_getchar();
        if (c == '\n') {
            buffer[pos] = '\0';
            vga_putchar('\n');
            vga_enable_cursor(); // Re-enable cursor after password input
            return buffer;
        } else if (c == '\b' && pos > 0) {
            pos--;
            vga_puts("\b \b");
        } else if (pos < size - 1 && c >= 32 && c <= 126) {
            buffer[pos++] = c;
            vga_putchar(' '); // Show spaces instead of asterisks
        }
    }
}

bool login_authenticate(void) {
    char username[USERNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
    int attempts = 0;

    while (attempts < MAX_LOGIN_ATTEMPTS) {
        login_get_username(username, sizeof(username));
        login_get_password(password, sizeof(password));

        if (strcmp(username, USERNAME) == 0 &&
            strcmp(password, PASSWORD) == 0) {
            return true;
        }

        printf("Login incorrect\n");
        attempts++;
    }

    return false;
}

void login_prompt(void) {
    printf("Welcome to the Unics ");
    printf(OS_VERSION);
    printf(" Operating System\n\n");

    if (!login_authenticate()) {
        printf("Maximum login attempts reached. Halting.\n");
        while (1) {} // Infinite loop
    }

}
