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

/* Simple FILE structure implementation */
static FILE stdin_file_struct = {0, 0, 0, 0};
static FILE stdout_file_struct = {1, 0, 0, 0};
static FILE stderr_file_struct = {2, 0, 0, 0};

FILE *stdin_file = &stdin_file_struct;
FILE *stdout_file = &stdout_file_struct;
FILE *stderr_file = &stderr_file_struct;

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

/* Helper function to get string length */
__hidden size_t __strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

/* Helper function to pad output */
__hidden void __pad_output(int count, char pad_char) {
    for (int i = 0; i < count; i++) {
        vga_putchar(pad_char);
    }
}

/* Helper function for formatted output with width/precision */
__hidden void __output_with_format(const char *str, int width, int precision, int left_align, int zero_pad) {
    int len = __strlen(str);
    
    /* Apply precision for strings */
    if (precision >= 0 && len > precision) {
        len = precision;
    }
    
    int pad_count = (width > len) ? width - len : 0;
    char pad_char = (zero_pad && !left_align) ? '0' : ' ';
    
    /* Right-aligned padding */
    if (!left_align && pad_count > 0) {
        __pad_output(pad_count, pad_char);
    }
    
    /* Output the string (with precision limit) */
    for (int i = 0; i < len; i++) {
        vga_putchar(str[i]);
    }
    
    /* Left-aligned padding */
    if (left_align && pad_count > 0) {
        __pad_output(pad_count, ' ');
    }
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

int putchar(int c) {
    vga_putchar(c);
    return c;
}

int puts(const char *str) {
    vga_puts(str);
    vga_putchar('\n');
    return 0;
}

/* Enhanced scanf with better error handling */
int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    int c;
    int *int_ptr;
    unsigned int *uint_ptr;
    long *long_ptr;
    char *str_ptr;
    char *char_ptr;
    
    while (*format) {
        /* Skip whitespace in format */
        while (__isspace(*format)) format++;

        if (*format == '%') {
            format++;
            
            /* Skip whitespace in input */
            while (__isspace((c = getchar()))) {}
            ungetc(c, 0);

            /* Handle length modifiers */
            int is_long = 0;
            int is_short = 0;
            (void)is_short;
            if (*format == 'l') {
                is_long = 1;
                format++;
            } else if (*format == 'h') {
                is_short = 1;
                format++;
            }

            switch (*format) {
                case 'd':
                case 'i': {
                    if (is_long) {
                        long_ptr = va_arg(args, long*);
                        *long_ptr = 0;
                    } else {
                        int_ptr = va_arg(args, int*);
                        *int_ptr = 0;
                    }
                    
                    int sign = 1;
                    long value = 0;
                    
                    c = getchar();
                    if (c == '-') {
                        sign = -1;
                        c = getchar();
                    } else if (c == '+') {
                        c = getchar();
                    }
                    
                    if (!__isdigit(c)) {
                        ungetc(c, 0);
                        goto scanf_done;
                    }
                    
                    while (__isdigit(c)) {
                        value = value * 10 + (c - '0');
                        c = getchar();
                    }
                    
                    value *= sign;
                    if (is_long) {
                        *long_ptr = value;
                    } else {
                        *int_ptr = (int)value;
                    }
                    
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case 'u': {
                    uint_ptr = va_arg(args, unsigned int*);
                    *uint_ptr = 0;
                    c = getchar();
                    
                    if (!__isdigit(c)) {
                        ungetc(c, 0);
                        goto scanf_done;
                    }
                    
                    while (__isdigit(c)) {
                        *uint_ptr = *uint_ptr * 10 + (c - '0');
                        c = getchar();
                    }
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case 'x':
                case 'X': {
                    int_ptr = va_arg(args, int*);
                    *int_ptr = 0;
                    c = getchar();
                    
                    /* Handle optional 0x prefix */
                    if (c == '0') {
                        c = getchar();
                        if (c == 'x' || c == 'X') {
                            c = getchar();
                        } else {
                            ungetc(c, 0);
                            c = '0';
                        }
                    }
                    
                    if (!__isxdigit(c)) {
                        ungetc(c, 0);
                        goto scanf_done;
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
                    char_ptr = va_arg(args, char*);
                    c = getchar();
                    if (c == EOF) goto scanf_done;
                    *char_ptr = c;
                    count++;
                    break;
                }
                
                case 's': {
                    str_ptr = va_arg(args, char*);
                    
                    /* Skip leading whitespace */
                    while (__isspace((c = getchar()))) {}
                    
                    if (c == EOF) {
                        goto scanf_done;
                    }
                    
                    /* Read until whitespace or EOF */
                    while (!__isspace(c) && c != EOF) {
                        *str_ptr++ = c;
                        c = getchar();
                    }
                    *str_ptr = '\0';
                    ungetc(c, 0);
                    count++;
                    break;
                }
                
                case '%': {
                    c = getchar();
                    if (c != '%') {
                        ungetc(c, 0);
                        goto scanf_done;
                    }
                    break;
                }
                
                default:
                    goto scanf_done;
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
    
scanf_done:
    va_end(args);
    return count;
}

/* Enhanced printf with format specifiers */
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int vprintf(const char *format, va_list args) {
    char str[64];
    int chars_written = 0;
    
    for (const char *ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr == '%' && *(ptr + 1) != '\0') {
            ptr++;
            
            /* Parse flags */
            int left_align = 0, zero_pad = 0, show_sign = 0, space_pad = 0, alternate = 0;
            while (*ptr == '-' || *ptr == '0' || *ptr == '+' || *ptr == ' ' || *ptr == '#') {
                switch (*ptr) {
                    case '-': left_align = 1; break;
                    case '0': if (!left_align) zero_pad = 1; break;
                    case '+': show_sign = 1; break;
                    case ' ': space_pad = 1; break;
                    case '#': alternate = 1; break;
                }
                ptr++;
            }
            
            /* Parse width */
            int width = 0;
            if (*ptr == '*') {
                width = va_arg(args, int);
                if (width < 0) {
                    left_align = 1;
                    width = -width;
                }
                ptr++;
            } else {
                while (__isdigit(*ptr)) {
                    width = width * 10 + (*ptr - '0');
                    ptr++;
                }
            }
            
            /* Parse precision */
            int precision = -1;
            if (*ptr == '.') {
                ptr++;
                precision = 0;
                if (*ptr == '*') {
                    precision = va_arg(args, int);
                    if (precision < 0) precision = -1;
                    ptr++;
                } else {
                    while (__isdigit(*ptr)) {
                        precision = precision * 10 + (*ptr - '0');
                        ptr++;
                    }
                }
            }
            
            /* Parse length modifier */
            int is_long = 0, is_short = 0;
            (void)is_short;
            if (*ptr == 'l') {
                is_long = 1;
                ptr++;
            } else if (*ptr == 'h') {
                is_short = 1;
                ptr++;
            }
            
            /* Handle format specifiers */
            switch (*ptr) {
                case 'z':
                    if (*(ptr + 1) == 'u') {
                        ptr++; // skip 'u'
                        size_t num = va_arg(args, size_t);
                        itoa(num, str, 10);
                        __output_with_format(str, width, precision, left_align, zero_pad);
                        chars_written += (size_t)width > __strlen(str) ? width : (int)__strlen(str);
                    }
                    break;
                    
                case 's': {
                    const char *s = va_arg(args, const char *);
                    if (!s) s = "(null)";
                    __output_with_format(s, width, precision, left_align, 0);
                    int len = __strlen(s);
                    if (precision >= 0 && len > precision) len = precision;
                    chars_written += width > len ? width : len;
                    break;
                }
                
                case 'd':
                case 'i': {
                    long num;
                    if (is_long) {
                        num = va_arg(args, long);
                    } else {
                        num = va_arg(args, int);
                    }
                    
                    char sign_char = 0;
                    if (num < 0) {
                        sign_char = '-';
                        num = -num;
                    } else if (show_sign) {
                        sign_char = '+';
                    } else if (space_pad) {
                        sign_char = ' ';
                    }
                    
                    itoa(num, str, 10);
                    
                    if (sign_char) {
                        /* Handle sign with zero padding */
                        if (zero_pad && !left_align && width > 0) {
                            vga_putchar(sign_char);
                            __pad_output(width - __strlen(str) - 1, '0');
                            vga_puts(str);
                            chars_written += width;
                        } else {
                            char temp[65];
                            temp[0] = sign_char;
                            strcpy(temp + 1, str);
                            __output_with_format(temp, width, precision, left_align, zero_pad);
                            int temp_len = (int)__strlen(temp);
                            chars_written += width > temp_len ? width : temp_len;
                        }
                    } else {
                        __output_with_format(str, width, precision, left_align, zero_pad);
                        chars_written += width > (int)__strlen(str) ? width : (int)__strlen(str);
                    }
                    break;
                }
                
                case 'x':
                case 'X': {
                unsigned int num = va_arg(args, unsigned int);
        itoa(num, str, 16);

        int len = __strlen(str);

        // Prefix "0x" if alternate form
        if (alternate && num != 0) {
            vga_puts("0x");
            chars_written += 2;
            width -= 2;
        }

        if (precision >= 0 && precision > len) {
        __pad_output(precision - len, '0');
        chars_written += (precision - len);
        } else if (zero_pad && !left_align && width > len) {
            __pad_output(width - len, '0');
            chars_written += (width - len);
                  }

                vga_puts(str);
                    chars_written += len;

                       if (left_align && width > len) {
                          __pad_output(width - len, ' ');
                    chars_written += (width - len);
                  }

                    break;
                }

                
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (width > 1) {
                        if (left_align) {
                            vga_putchar(c);
                            __pad_output(width - 1, ' ');
                        } else {
                            __pad_output(width - 1, ' ');
                            vga_putchar(c);
                        }
                        chars_written += width;
                    } else {
                        vga_putchar(c);
                        chars_written++;
                    }
                    break;
                }
                
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    itoa(num, str, 10);
                    __output_with_format(str, width, precision, left_align, zero_pad);
                    chars_written += width > (int)__strlen(str) ? width : (int)__strlen(str);
                    break;
                }
                
                case 'p': {
                    void *ptr_val = va_arg(args, void *);
                    vga_puts("0x");
                    itoa((uintptr_t)ptr_val, str, 16);
                    vga_puts(str);
                    chars_written += 2 + __strlen(str);
                    break;
                }
                
                case '%': {
                    vga_putchar('%');
                    chars_written++;
                    break;
                }
                
                default:
                    vga_putchar('%');
                    vga_putchar(*ptr);
                    chars_written += 2;
                    break;
            }
        } else {
            vga_putchar(*ptr);
            chars_written++;
        }
    }
    
    return chars_written;
}

int fprintf(int fd, const char *format, ...) {
    (void)fd;  // Add this to silence the warning
    va_list args;
    va_start(args, format);
    /* For now, all output goes to VGA regardless of fd */
    int result = vprintf(format, args);
    va_end(args);
    return result;
}

int sprintf(char *buf, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(buf, format, args);
    va_end(args);
    return result;
}

int vsprintf(char *buf, const char *format, va_list args) {
    char *dst = buf;
    char num_buf[20];

    for (const char *ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr != '%' || !*(ptr + 1)) {
            *dst++ = *ptr;
            continue;
        }
        
        ptr++;
        
        /* Parse width (simple version) */
        int width = 0;
        while (__isdigit(*ptr)) {
            width = width * 10 + (*ptr - '0');
            ptr++;
        }
        
        switch (*ptr) {
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                while (*s) *dst++ = *s++;
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(args, int);
                itoa(num, num_buf, 10);
                for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
                break;
            }
            case 'x':
            case 'X': {
                int num = va_arg(args, int);
                itoa(num, num_buf, 16);
                for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
                break;
            }
            case 'u': {
                unsigned int num = va_arg(args, unsigned int);
                itoa(num, num_buf, 10);
                for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
                break;
            }
            case 'c': {
                *dst++ = (char)va_arg(args, int);
                break;
            }
            case 'p': {
                *dst++ = '0'; *dst++ = 'x';
                itoa((int)(uintptr_t)va_arg(args, void *), num_buf, 16);
                for (int i = 0; num_buf[i]; i++) *dst++ = num_buf[i];
                break;
            }
            case '%': {
                *dst++ = '%';
                break;
            }
            default: {
                *dst++ = '%';
                *dst++ = *ptr;
                break;
            }
        }
    }

    *dst = '\0';
    return (dst - buf);
}

int snprintf(char *buf, size_t n, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buf, n, format, args);
    va_end(args);
    return result;
}

int vsnprintf(char *buf, size_t n, const char *format, va_list args) {
    char *dst = buf;
    const char *end = buf + n - 1;
    char num_buf[20];

    for (const char *ptr = format; *ptr && dst < end; ptr++) {
        if (*ptr != '%' || !*(ptr + 1)) {
            *dst++ = *ptr;
            continue;
        }
        
        ptr++;
        
        switch (*ptr) {
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                while (*s && dst < end) *dst++ = *s++;
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(args, int);
                itoa(num, num_buf, 10);
                for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
                break;
            }
            case 'x':
            case 'X': {
                int num = va_arg(args, int);
                itoa(num, num_buf, 16);
                for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
                break;
            }
            case 'u': {
                unsigned int num = va_arg(args, unsigned int);
                itoa(num, num_buf, 10);
                for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
                break;
            }
            case 'c': {
                if (dst < end) *dst++ = (char)va_arg(args, int);
                else (void)va_arg(args, int); /* consume argument */
                break;
            }
            case 'p': {
                if (dst + 2 < end) {
                    *dst++ = '0'; *dst++ = 'x';
                    itoa((int)(uintptr_t)va_arg(args, void *), num_buf, 16);
                    for (int i = 0; num_buf[i] && dst < end; i++) *dst++ = num_buf[i];
                } else {
                    (void)va_arg(args, void*); /* consume argument */
                }
                break;
            }
            case '%': {
                if (dst < end) *dst++ = '%';
                break;
            }
            default: {
                if (dst < end) *dst++ = '%';
                if (dst < end) *dst++ = *ptr;
                break;
            }
        }
    }

    if (n > 0) *dst = '\0';
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

    while (*format && str[i]) {
        while (__isspace(*format)) format++;

        if (*format == '%') {
            format++;
            
            /* Parse width */
            int width = 0;
            while (__isdigit(*format)) {
                width = width * 10 + (*format - '0');
                format++;
            }
            
            switch (*format) {
                case 'd':
                case 'i': {
                    int *int_ptr = va_arg(args, int*);
                    int sign = 1;
                    int num = 0;
                    int digits = 0;

                    /* Skip whitespace */
                    while (__isspace(str[i])) i++;

                    if (str[i] == '-') {
                        sign = -1;
                        i++;
                    } else if (str[i] == '+') {
                        i++;
                    }

                    while (__isdigit(str[i]) && (width == 0 || digits < width)) {
                        num = num * 10 + (str[i] - '0');
                        i++;
                        digits++;
                    }

                    if (digits == 0) goto sscanf_done;
                    *int_ptr = num * sign;
                    count++;
                    break;
                }
                case 'u': {
                    unsigned int *uint_ptr = va_arg(args, unsigned int*);
                    unsigned int num = 0;
                    int digits = 0;

                    while (__isspace(str[i])) i++;

                    while (__isdigit(str[i]) && (width == 0 || digits < width)) {
                        num = num * 10 + (str[i] - '0');
                        i++;
                        digits++;
                    }

                    if (digits == 0) goto sscanf_done;
                    *uint_ptr = num;
                    count++;
                    break;
                }
                case 'x':
                case 'X': {
                    int *int_ptr = va_arg(args, int*);
                    int num = 0;
                    int digits = 0;

                    while (__isspace(str[i])) i++;

                    /* Skip optional 0x prefix */
                    if (str[i] == '0' && (str[i+1] == 'x' || str[i+1] == 'X')) {
                        i += 2;
                    }

                    while (__isxdigit(str[i]) && (width == 0 || digits < width)) {
                        if (__isdigit(str[i])) {
                            num = num * 16 + (str[i] - '0');
                        } else {
                            num = num * 16 + (__tolower(str[i]) - 'a' + 10);
                        }
                        i++;
                        digits++;
                    }

                    if (digits == 0) goto sscanf_done;
                    *int_ptr = num;
                    count++;
                    break;
                }
                case 'c': {
                    char *char_ptr = va_arg(args, char*);
                    if (!str[i]) goto sscanf_done;
                    *char_ptr = str[i++];
                    count++;
                    break;
                }
                case 's': {
                    char *str_ptr = va_arg(args, char*);
                    int chars = 0;
                    
                    /* Skip whitespace */
                    while (__isspace(str[i])) i++;

                    /* Read non-whitespace characters */
                    while (!__isspace(str[i]) && str[i] != '\0' && (width == 0 || chars < width)) {
                        *str_ptr++ = str[i++];
                        chars++;
                    }
                    
                    if (chars == 0) goto sscanf_done;
                    *str_ptr = '\0';
                    count++;
                    break;
                }
                case '%': {
                    if (str[i] != '%') goto sscanf_done;
                    i++;
                    break;
                }
                default:
                    goto sscanf_done;
            }
            format++;
        } else {
            /* Match literal characters */
            if (str[i] != *format) break;
            i++;
            format++;
        }
    }

sscanf_done:
    va_end(args);
    return count;
}

/* String input functions */
char *gets(char *s) {
    int i = 0;
    int c;
    
    while ((c = getchar()) != '\n' && c != EOF) {
        s[i++] = c;
    }
    s[i] = '\0';
    
    return (c == EOF && i == 0) ? NULL : s;
}

char *fgets(char *s, int size, FILE *stream) {
    (void)stream; /* Currently unused */
    int i = 0;
    int c;
    
    while (i < size - 1 && (c = getchar()) != '\n' && c != EOF) {
        s[i++] = c;
    }
    
    if (c == '\n') {
        s[i++] = '\n';
    }
    s[i] = '\0';
    
    return (c == EOF && i == 0) ? NULL : s;
}

/* File I/O functions (basic implementation) */
int fgetc(FILE *stream) {
    if (stream == stdin_file) {
        return getchar();
    }
    /* For other streams, return EOF for now */
    return EOF;
}

int fputc(int c, FILE *stream) {
    if (stream == stdout_file || stream == stderr_file) {
        return putchar(c);
    }
    /* For other streams, return EOF for now */
    return EOF;
}

/* Error handling functions */
void perror(const char *s) {
    if (s && *s) {
        vga_puts(s);
        vga_puts(": ");
    }
    vga_puts("Error occurred\n");
}

int ferror(FILE *stream) {
    return stream ? stream->error : 1;
}

int feof(FILE *stream) {
    return stream ? stream->eof : 1;
}

void clearerr(FILE *stream) {
    if (stream) {
        stream->error = 0;
        stream->eof = 0;
    }
}

/* File positioning functions (stub implementations) */
long ftell(FILE *stream) {
    (void)stream;
    return -1L; /* Not implemented */
}

int fseek(FILE *stream, long offset, int whence) {
    (void)stream;
    (void)offset;
    (void)whence;
    return -1; /* Not implemented */
}

void rewind(FILE *stream) {
    (void)stream;
    /* Not implemented */
}

/* Buffer control functions (stub implementations) */
int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
    (void)stream;
    (void)buf;
    (void)mode;
    (void)size;
    return 0; /* Success (but not actually implemented) */
}

void setbuf(FILE *stream, char *buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}

__END_DECLS