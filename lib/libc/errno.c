#include <errno.h>
#include <sys/cdefs.h>

/* 
 * Simple implementation for kernel-space.
 * For user-space, you'd want thread-local storage.
 */
static int __errno_storage;

int *__errno_location(void) {
    return &__errno_storage;
}
