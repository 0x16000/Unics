#ifndef LOGIN_H
#define LOGIN_H

#include <stdbool.h>
#include <vga.h>
#include <keyboard.h>

#define MAX_LOGIN_ATTEMPTS 3
#define USERNAME "root"
#define PASSWORD "welcome"
#define USERNAME_MAX_LEN 32
#define PASSWORD_MAX_LEN 32

bool login_authenticate(void);
void login_prompt(void);
char* login_get_username(char* buffer, size_t size);
char* login_get_password(char* buffer, size_t size);

#endif // LOGIN_H
