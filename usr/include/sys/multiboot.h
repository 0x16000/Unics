#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003  // Align modules on page boundaries and provide memory map
#define MULTIBOOT_INFO_MAGIC   0x2BADB002
#define MULTIBOOT_INFO_MEMORY  0x00000001

// Multiboot header structure
struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    // Additional fields can be added here if needed
};

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;    // Memory below 1MB (in KB)
    uint32_t mem_upper;    // Memory above 1MB (in KB)
    // Add other fields as needed
};

#endif // MULTIBOOT_H
