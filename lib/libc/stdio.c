#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vga.h>
#include <keyboard.h>
#include <sys/cdefs.h>
#include <sys/ctype.h>

__BEGIN_DECLS

/* Single character pushback buffer */
static int unget_char = EOF;

/* Character classification functions - moved to ctype.h */
__hidden int __isdigit(int c) {
    return (c >= '0' && c <= '9');
}

__hidden int __isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || 
            c == '\r' || c == '\f' || c == '\v');
}

__hidden int __isxdigit(int c) {
    return __isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

__hidden int __tolower(int c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

/* Standard I/O functions */
int getchar(void) {
    if (unget_char != EOF) {
        int c = unget_char;
        unget_char = EOF;
        return c;
    }
    return kb_getchar();
}

int ungetc(int c, int fd) {
    (void)fd;
    if (unget_char != EOF) return EOF;
    if (c != EOF) unget_char = c;
    return c;
}

int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    int c;
    int *int_ptr;
    unsigned int *uint_ptr;
    char *str_ptr;
    
    while (*format) {
        while (__isspace(*format)) format++;

        if (*format == '%') {
            format++;
            
            while (__isspace((c = getchar()))) {}
            ungetc(c, 0);

            switch (*format) {
                case 'd': {
                    int_ptr = va_arg(args, int*);
                    *int_ptr = 0;
                    int sign = 1;
                    
                    c = getchar();
                    if (c == '-') {
                        sign = -1;
                        c = getchar();
                    } else if (c == '+') {
                        c = getchar();
                    }
                    
                    while (__isdigit(c)) {
                        *int_ptr = *int_ptr * 10 + (c - '0');
                        c = getchar();
                    }
                    *int_ptr *= sign;
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case 'u': {
                    uint_ptr = va_arg(args, unsigned int*);
                    *uint_ptr = 0;
                    c = getchar();
                    
                    while (__isdigit(c)) {
                        *uint_ptr = *uint_ptr * 10 + (c - '0');
                        c = getchar();
                    }
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case 'x': {
                    int_ptr = va_arg(args, int*);
                    *int_ptr = 0;
                    c = getchar();
                    
                    if (c == '0') {
                        c = getchar();
                        if (c != 'x' && c != 'X') {
                            ungetc(c, 0);
                            c = '0';
                        } else {
                            c = getchar();
                        }
                    }
                    
                    while (__isxdigit(c)) {
                        if (__isdigit(c)) {
                            *int_ptr = *int_ptr * 16 + (c - '0');
                        } else {
                            *int_ptr = *int_ptr * 16 + (__tolower(c) - 'a' + 10);
                        }
                        c = getchar();
                    }
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case 'c': {
                    int_ptr = va_arg(args, int*);
                    *int_ptr = getchar();
                    count++;
                    break;
                }
                
                case 's': {
                    str_ptr = va_arg(args, char*);
                    
                    while (__isspace((c = getchar()))) {}
                    
                    while (!__isspace(c) && c != EOF) {
                        *str_ptr++ = c;
                        c = getchar();
                    }
                    *str_ptr = '\0';
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                default:
                    break;
            }
            format++;
        } else {
            c = getchar();
            if (c != *format) {
                ungetc(c, 0);
                break;
            }
            format++;
        }
    }
    
    va_end(args);
    return count;
}

int putchar(int c) {
    vga_putchar(c);
    return c;
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char str[20];

    for (const char *ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr == '%' && *(ptr + 1) != '\0') {
            ptr++;
            
            if (*ptr == 's') {
                const char *s = va_arg(args, const char *);
                vga_puts(s ? s : "(null)");
            } else if (*ptr == 'd' || *ptr == 'i') {
                int num = va_arg(args, int);
                itoa(num, str, 10);
                vga_puts(str);
            } else if (*ptr == 'x' || *ptr == 'X') {
                int num = va_arg(args, int);
                itoa(num, str, 16);
                vga_puts(str);
            } else if (*ptr == 'c') {
                char c = (char)va_arg(args, int);
                vga_putchar(c);
            } else if (*ptr == 'u') {
                unsigned int num = va_arg(args, unsigned int);
                if (num <= 0x7FFFFFFF) {
                    itoa((int)num, str, 10);
                } else {
                    itoa(214748364, str, 10);
                    vga_puts(str);
                    itoa((int)(num - 2147483640), str, 10);
                }
                vga_puts(str);
            } else if (*ptr == 'p') {
                void *ptr_val = va_arg(args, void *);
                vga_puts("0x");
                itoa((int)(uintptr_t)ptr_val, str, 16);
                vga_puts(str);
            } else if (*ptr == '%') {
                vga_putchar('%');
            } else {
                vga_putchar('%');
                vga_putchar(*ptr);
            }
        } else {
            vga_putchar(*ptr);
        }
    }

    va_end(args);
    return 0;
}

int puts(const char *str) {
    vga_puts(str);
    vga_putchar('\n');
    return 0;
}

int sprintf(char *buf, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *dst = buf;
    char num_buf[20];

    for (const char *ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr != '%' || !*(ptr + 1)) {
            *dst++ = *ptr;
            continue;
        }
        
        ptr++;
        
        if (*ptr == 's') {
            const char *s = va_arg(args, const char *);
            if (!s) s = "(null)";
            while (*s) *dst++ = *s++;
        } else if (*ptr == 'd' || *ptr == 'i') {
            int num = va_arg(args, int);
            itoa(num, num_buf, 10);
            for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'x' || *ptr == 'X') {
            int num = va_arg(args, int);
            itoa(num, num_buf, 16);
            for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'u') {
            unsigned int num = va_arg(args, unsigned int);
            if (num <= 0x7FFFFFFF) {
                itoa((int)num, num_buf, 10);
            } else {
                *dst++ = '2'; *dst++ = '1'; *dst++ = '4';
                *dst++ = '7'; *dst++ = '4'; *dst++ = '8';
                *dst++ = '3'; *dst++ = '6'; *dst++ = '4';
                *dst++ = '0';
                itoa((int)(num - 2147483640), num_buf, 10);
            }
            for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'c') {
            *dst++ = (char)va_arg(args, int);
        } else if (*ptr == 'p') {
            *dst++ = '0'; *dst++ = 'x';
            itoa((int)(uintptr_t)va_arg(args, void *), num_buf, 16);
            for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
        } else if (*ptr == '%') {
            *dst++ = '%';
        } else {
            *dst++ = '%'; *dst++ = *ptr;
        }
    }

    *dst = '\0';
    va_end(args);
    return (dst - buf);
}

int snprintf(char *buf, size_t n, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *dst = buf;
    const char *end = buf + n - 1;
    char num_buf[20];

    for (const char *ptr = format; *ptr && dst < end; ptr++) {
        if (*ptr != '%' || !*(ptr + 1)) {
            *dst++ = *ptr;
            continue;
        }
        
        ptr++;
        
        if (*ptr == 's') {
            const char *s = va_arg(args, const char *);
            if (!s) s = "(null)";
            while (*s && dst < end) *dst++ = *s++;
        } else if (*ptr == 'd' || *ptr == 'i') {
            int num = va_arg(args, int);
            itoa(num, num_buf, 10);
            for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'x' || *ptr == 'X') {
            int num = va_arg(args, int);
            itoa(num, num_buf, 16);
            for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'u') {
            unsigned int num = va_arg(args, unsigned int);
            if (num <= 0x7FFFFFFF) {
                itoa((int)num, num_buf, 10);
            } else if (dst + 10 < end) {
                *dst++ = '2'; *dst++ = '1'; *dst++ = '4';
                *dst++ = '7'; *dst++ = '4'; *dst++ = '8';
                *dst++ = '3'; *dst++ = '6'; *dst++ = '4';
                *dst++ = '0';
                itoa((int)(num - 2147483640), num_buf, 10);
            }
            for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
        } else if (*ptr == 'c') {
            if (dst < end) *dst++ = (char)va_arg(args, int);
        } else if (*ptr == 'p') {
            if (dst + 2 < end) {
                *dst++ = '0'; *dst++ = 'x';
                itoa((int)(uintptr_t)va_arg(args, void *), num_buf, 16);
                for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
            }
        } else if (*ptr == '%') {
            if (dst < end) *dst++ = '%';
        } else {
            if (dst < end) *dst++ = '%';
            if (dst < end) *dst++ = *ptr;
        }
    }

    *dst = '\0';
    va_end(args);
    return (dst - buf);
}

int fputs(const char *str, int fd) {
    (void)fd;
    vga_puts(str);
    return 0;
}

int sscanf(const char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int count = 0;
    int i = 0; // String index for the input string

    while (*format) {
        while (isspace((unsigned char)*format)) { // Skip whitespace
            format++;
        }

        if (*format == '%') {
            format++;
            if (*format == 'd') {
                // Parse integer
                int *int_ptr = va_arg(args, int*);
                int sign = 1;
                int num = 0;

                if (str[i] == '-') {
                    sign = -1;
                    i++;
                } else if (str[i] == '+') {
                    i++;
                }

                while (isdigit((unsigned char)str[i])) {
                    num = num * 10 + (str[i] - '0');
                    i++;
                }

                *int_ptr = num * sign;
                count++;
            }
            else if (*format == 'u') {
                // Parse unsigned integer
                unsigned int *uint_ptr = va_arg(args, unsigned int*);
                unsigned int num = 0;

                while (isdigit((unsigned char)str[i])) {
                    num = num * 10 + (str[i] - '0');
                    i++;
                }

                *uint_ptr = num;
                count++;
            }
            else if (*format == 's') {
                // Parse string
                char *str_ptr = va_arg(args, char*);
                while (isspace((unsigned char)str[i])) { // Skip whitespace
                    i++;
                }

                int j = 0;
                while (!isspace((unsigned char)str[i]) && str[i] != '\0') {
                    str_ptr[j++] = str[i++];
                }
                str_ptr[j] = '\0';
                count++;
            }
            else {
                // Handle other format specifiers as needed
            }
            format++;
        }
        else {
            // Match literal characters
            if (str[i] != *format) {
                break;
            }
            i++;
            format++;
        }
    }

    va_end(args);
    return count;
}

__END_DECLS
