#ifndef _IO_H
#define _IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read a byte from the specified port
 * @param port The I/O port to read from
 * @return The byte read from the port
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov dx, port
        in al, dx
        mov ret, al
    }
    #else
    #error "Unsupported compiler for inb()"
    #endif
    return ret;
}

/**
 * @brief Write a byte to the specified port
 * @param port The I/O port to write to
 * @param val The byte to write
 */
static inline void outb(uint16_t port, uint8_t val) {
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov al, val
        mov dx, port
        out dx, al
    }
    #else
    #error "Unsupported compiler for outb()"
    #endif
}

/**
 * @brief Read a word (16 bits) from the specified port
 * @param port The I/O port to read from
 * @return The word read from the port
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov dx, port
        in ax, dx
        mov ret, ax
    }
    #else
    #error "Unsupported compiler for inw()"
    #endif
    return ret;
}

/**
 * @brief Write a word (16 bits) to the specified port
 * @param port The I/O port to write to
 * @param val The word to write
 */
static inline void outw(uint16_t port, uint16_t val) {
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov ax, val
        mov dx, port
        out dx, ax
    }
    #else
    #error "Unsupported compiler for outw()"
    #endif
}

/**
 * @brief Read a double word (32 bits) from the specified port
 * @param port The I/O port to read from
 * @return The double word read from the port
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov dx, port
        in eax, dx
        mov ret, eax
    }
    #else
    #error "Unsupported compiler for inl()"
    #endif
    return ret;
}

/**
 * @brief Write a double word (32 bits) to the specified port
 * @param port The I/O port to write to
 * @param val The double word to write
 */
static inline void outl(uint16_t port, uint32_t val) {
    #if defined(__GNUC__) || defined(__clang__)
    asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
    #elif defined(_MSC_VER)
    __asm {
        mov eax, val
        mov dx, port
        out dx, eax
    }
    #else
    #error "Unsupported compiler for outl()"
    #endif
}

/**
 * @brief Wait for a short time for I/O to complete
 * This is typically used after an outb to a slow device
 */
static inline void io_wait(void) {
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    outb(0x80, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* _IO_H */
