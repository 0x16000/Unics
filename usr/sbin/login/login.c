#include <login.h>
#include <string.h>
#include <vga.h>
#include <keyboard.h>
#include <stdio.h>
#include <vers.h>
#include <time.h>

#define PASSWORD_MASK '*'
#define LOGIN_DELAY_MS 2000  // 2 second delay after failed attempts
#define INPUT_TIMEOUT_MS 300000

// Enhanced input buffer clearing for security
static void secure_clear_buffer(volatile char* buffer, size_t size) {
    // Use volatile to prevent compiler optimization
    volatile char* ptr = buffer;
    for (size_t i = 0; i < size; i++) {
        ptr[i] = 0;
    }
}

// Input validation helper
static bool is_valid_input_char(char c) {
    return (c >= 32 && c <= 126) && c != '\0';
}

// Enhanced input function with better error handling
static int get_input_secure(char* buffer, size_t size, bool is_password) {
    if (!buffer || size == 0) {
        return -1;  // Invalid parameters
    }
    
    size_t pos = 0;
    secure_clear_buffer(buffer, size);
    
    while (true) {
        char c = kb_getchar();
        
        // Handle different input cases
        switch (c) {
            case '\n':
            case '\r':
                buffer[pos] = '\0';
                if (!is_password) {
                    vga_putchar('\n');
                }
                return 0;  // Success
                
            case '\b':
            case 127:  // DEL key
                if (pos > 0) {
                    pos--;
                    buffer[pos] = '\0';  // Clear the character
                    if (!is_password) {
                        // Move cursor back, overwrite with space, move back again
                        vga_putchar('\b');
                        vga_putchar(' ');
                        vga_putchar('\b');
                    } else {
                        // For password, still need to move cursor back and clear
                        vga_putchar('\b');
                        vga_putchar(' ');
                        vga_putchar('\b');
                    }
                }
                break;
                
            case 3:    // Ctrl+C
            case 27:   // ESC
                secure_clear_buffer(buffer, size);
                return -2;  // User cancelled
                
            default:
                if (pos < size - 1 && is_valid_input_char(c)) {
                    buffer[pos++] = c;
                    vga_putchar(is_password ? PASSWORD_MASK : c);
                }
                break;
        }
    }
}

// Improved username input with validation
char* login_get_username(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return NULL;
    }
    
    printf("login: ");
    vga_enable_cursor();
    
    int result = get_input_secure(buffer, size, false);
    if (result != 0) {
        secure_clear_buffer(buffer, size);
        return NULL;
    }
    
    // Basic username validation
    if (strlen(buffer) == 0) {
        return NULL;
    }
    
    return buffer;
}

// Improved password input with better security
char* login_get_password(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return NULL;
    }
    
    printf("password: ");
    vga_disable_cursor();
    
    int result = get_input_secure(buffer, size, true);
    
    vga_enable_cursor();
    vga_putchar('\n');  // Always add newline after password
    
    if (result != 0) {
        secure_clear_buffer(buffer, size);
        return NULL;
    }
    
    return buffer;
}

// Enhanced authentication with better security practices
bool login_authenticate(void) {
    char username[USERNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
    int attempts = 0;
    bool auth_success = false;
    
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        // Clear buffers before each attempt
        secure_clear_buffer(username, sizeof(username));
        secure_clear_buffer(password, sizeof(password));
        
        // Get credentials
        if (login_get_username(username, sizeof(username)) == NULL) {
            printf("Invalid username input\n");
            attempts++;
            continue;
        }
        
        if (login_get_password(password, sizeof(password)) == NULL) {
            printf("Invalid password input\n");
            attempts++;
            continue;
        }
        
        // Authenticate (consider using constant-time comparison for passwords)
        if (strcmp(username, USERNAME) == 0 && strcmp(password, PASSWORD) == 0) {
            auth_success = true;
            break;
        }
        
        attempts++;
        
        // Security measures after failed attempt
        if (attempts < MAX_LOGIN_ATTEMPTS) {
            printf("Login incorrect\n");
            // Add delay to prevent brute force attacks
            delay(INPUT_TIMEOUT_MS);
        }
    }
    
    // Securely clear sensitive data
    secure_clear_buffer(username, sizeof(username));
    secure_clear_buffer(password, sizeof(password));
    
    return auth_success;
}

// Enhanced login prompt with better error handling
void login_prompt(void) {

    if (!login_authenticate()) {
        printf("Maximum login attempts reached.\n");
        printf("System access denied.\n");
        
        // Better halt mechanism - could be replaced with proper system halt
        vga_disable_cursor();
        printf("System halted. Please restart.\n");
        
        // Infinite loop with potential for watchdog reset
        while (1) {
            __asm__("hlt");
        }
    }
    
    // Successful login
    printf("Login successful. Welcome!\n");
}
