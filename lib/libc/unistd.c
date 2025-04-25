#include <stdint.h>
#include <sys/syscall.h>

intptr_t syscall0(int num) {
    int result;
    __asm__ (
        "int $0x80"          // Trigger interrupt for system call
        : "=a" (result)      // Store result in eax
        : "0" (num)          // Pass syscall number in eax
    );
    return (intptr_t)result;
}

intptr_t syscall1(int num, intptr_t arg1) {
    int result;
    __asm__ (
        "int $0x80"          // Trigger interrupt for system call
        : "=a" (result)      // Store result in eax
        : "0" (num), "b" (arg1) // Pass syscall number in eax, arg1 in ebx
    );
    return (intptr_t)result;
}

intptr_t syscall2(int num, intptr_t arg1, intptr_t arg2) {
    int result;
    __asm__ (
        "int $0x80"          // Trigger interrupt for system call
        : "=a" (result)      // Store result in eax
        : "0" (num), "b" (arg1), "c" (arg2) // Pass syscall number in eax, args in ebx and ecx
    );
    return (intptr_t)result;
}

intptr_t syscall3(int num, intptr_t arg1, intptr_t arg2, intptr_t arg3) {
    int result;
    __asm__ (
        "int $0x80"          // Trigger interrupt for system call
        : "=a" (result)      // Store result in eax
        : "0" (num), "b" (arg1), "c" (arg2), "d" (arg3) // Pass syscall number in eax, args in ebx, ecx, edx
    );
    return (intptr_t)result;
}

unsigned int sleep(unsigned int seconds) {
    return (unsigned int) syscall1(SYS_sleep, (intptr_t)seconds);
}
