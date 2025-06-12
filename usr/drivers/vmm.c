#include <vmm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

static vmm_page_t page_table[VMM_MAX_PAGES];
static bool vmm_initialized = false;

void vmm_init(void) {
    if (vmm_initialized) return;
    
    memset(page_table, 0, sizeof(page_table));
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        page_table[i].base_addr = VMM_BASE_ADDRESS + i * VMM_PAGE_SIZE;
        page_table[i].size = VMM_PAGE_SIZE;
        page_table[i].state = VMM_PAGE_FREE;
        page_table[i].alloc_pages = 0;
        page_table[i].tag = 0;
        page_table[i].is_kernel = false;
    }
    vmm_initialized = true;
    printf("vmm: Initialized %d pages (%.2f MB) starting at 0x%08x\n", 
           VMM_MAX_PAGES, (VMM_MAX_PAGES * VMM_PAGE_SIZE) / (1024.0 * 1024.0),
           (unsigned)VMM_BASE_ADDRESS);
}

void vmm_reserve_range(uintptr_t start, uintptr_t end) {
    assert(vmm_initialized && "VMM not initialized");
    
    // Align addresses to page boundaries
    start = start & ~(VMM_PAGE_SIZE - 1);
    end = (end + VMM_PAGE_SIZE - 1) & ~(VMM_PAGE_SIZE - 1);
    
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].base_addr >= start && 
            page_table[i].base_addr < end) {
            page_table[i].state = VMM_PAGE_RESERVED;
            page_table[i].tag = VMM_TAG_KERNEL;
            page_table[i].is_kernel = true;
        }
    }
}

static void* vmm_alloc_internal(size_t pages_needed, size_t align, uint32_t tag, bool is_kernel) {
    assert(vmm_initialized && "VMM not initialized");
    assert(pages_needed > 0 && pages_needed <= VMM_MAX_PAGES);
    
    for (size_t i = 0; i <= VMM_MAX_PAGES - pages_needed; ++i) {
        // Skip reserved pages
        if (page_table[i].state == VMM_PAGE_RESERVED) continue;
        
        // Check alignment if requested
        if (align > VMM_PAGE_SIZE) {
            uintptr_t aligned_addr = (page_table[i].base_addr + align - 1) & ~(align - 1);
            if (aligned_addr != page_table[i].base_addr) {
                // Check if we have enough space after alignment
                size_t offset_pages = (aligned_addr - page_table[i].base_addr) / VMM_PAGE_SIZE;
                if (i + offset_pages + pages_needed > VMM_MAX_PAGES) {
                    continue;
                }
                i += offset_pages - 1;  // -1 because loop will increment
                continue;
            }
        }
        
        // Check if block is free
        bool block_free = true;
        for (size_t j = 0; j < pages_needed; ++j) {
            if (page_table[i + j].state != VMM_PAGE_FREE) {
                block_free = false;
                i += j; // Skip past this used page
                break;
            }
        }
        
        if (block_free) {
            // Mark all pages as used
            for (size_t j = 0; j < pages_needed; ++j) {
                page_table[i + j].state = VMM_PAGE_USED;
                page_table[i + j].alloc_pages = (j == 0) ? pages_needed : 0;
                page_table[i + j].tag = tag;
                page_table[i + j].is_kernel = is_kernel;
            }
            return (void*)page_table[i].base_addr;
        }
    }
    
    return NULL;
}

void* vmm_alloc(size_t size) {
    if (size == 0) return NULL;
    
    size_t pages_needed = (size + VMM_PAGE_SIZE - 1) / VMM_PAGE_SIZE;
    if (pages_needed > VMM_MAX_PAGES) {
        printf("vmm: Error: Requested size too large (%zu pages)\n", pages_needed);
        return NULL;
    }
    
    return vmm_alloc_internal(pages_needed, VMM_PAGE_SIZE, 0, false);
}

void* vmm_alloc_aligned(size_t size, size_t align) {
    if (size == 0) return NULL;
    
    // Ensure alignment is power of 2
    if ((align & (align - 1)) != 0 || align == 0) {
        printf("vmm: Error: Alignment must be power of 2\n");
        return NULL;
    }
    
    size_t pages_needed = (size + VMM_PAGE_SIZE - 1) / VMM_PAGE_SIZE;
    if (pages_needed > VMM_MAX_PAGES) {
        printf("vmm: Error: Requested size too large (%zu pages)\n", pages_needed);
        return NULL;
    }
    
    return vmm_alloc_internal(pages_needed, align, 0, false);
}

void* vmm_alloc_tagged(size_t size, uint32_t tag, bool is_kernel) {
    if (size == 0) return NULL;
    
    size_t pages_needed = (size + VMM_PAGE_SIZE - 1) / VMM_PAGE_SIZE;
    if (pages_needed > VMM_MAX_PAGES) {
        printf("vmm: Error: Requested size too large (%zu pages)\n", pages_needed);
        return NULL;
    }
    
    return vmm_alloc_internal(pages_needed, VMM_PAGE_SIZE, tag, is_kernel);
}

void vmm_free(void* addr) {
    if (!addr || !vmm_initialized) return;

    uintptr_t target = (uintptr_t)addr;
    
    // Find the first page of the allocation
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].base_addr <= target && 
            target < page_table[i].base_addr + page_table[i].size) {
            
            if (page_table[i].state != VMM_PAGE_USED) {
                printf("vmm: Warning: Tried to free unallocated memory at 0x%08x\n", 
                      (unsigned)target);
                return;
            }

            // Verify the allocation is valid before freeing
            size_t pages = page_table[i].alloc_pages;
            if (pages == 0 || i + pages > VMM_MAX_PAGES) {
                printf("vmm: Error: Corrupted page table detected during free\n");
                return;
            }

            // Free all pages in the allocation
            for (size_t j = 0; j < pages; ++j) {
                if (page_table[i + j].state != VMM_PAGE_USED) {
                    printf("vmm: Error: Inconsistent page state during free\n");
                    return;
                }
                page_table[i + j].state = VMM_PAGE_FREE;
                page_table[i + j].alloc_pages = 0;
                page_table[i + j].tag = 0;
                page_table[i + j].is_kernel = false;
            }
            return;
        }
    }

    printf("vmm: Warning: Tried to free unallocated memory at 0x%08x\n", 
          (unsigned)target);
}

void vmm_dump_state(void) {
    assert(vmm_initialized && "VMM not initialized");
    
    size_t used = 0, free = 0, reserved = 0;
    printf("[vmm] Page table dump:\n");
    for (size_t i = 0; i < VMM_MAX_PAGES; ) {
        if (page_table[i].alloc_pages > 0) {
            // Print allocation block
            printf("  0x%08x-0x%08x: %4zu pages (%6.2f KB) - %s %s (tag: 0x%08x)\n",
                  (unsigned)page_table[i].base_addr,
                  (unsigned)(page_table[i].base_addr + page_table[i].alloc_pages * VMM_PAGE_SIZE - 1),
                  page_table[i].alloc_pages,
                  (page_table[i].alloc_pages * VMM_PAGE_SIZE) / 1024.0,
                  page_table[i].is_kernel ? "KERNEL" : "USER  ",
                  page_table[i].state == VMM_PAGE_USED ? "USED" : 
                  page_table[i].state == VMM_PAGE_RESERVED ? "RESERVED" : "FREE",
                  page_table[i].tag);
            
            if (page_table[i].state == VMM_PAGE_USED) used += page_table[i].alloc_pages;
            else if (page_table[i].state == VMM_PAGE_RESERVED) reserved += page_table[i].alloc_pages;
            else free += page_table[i].alloc_pages;
            
            i += page_table[i].alloc_pages;
        } else {
            // Single page
            if (page_table[i].state == VMM_PAGE_USED) used++;
            else if (page_table[i].state == VMM_PAGE_RESERVED) reserved++;
            else free++;
            
            i++;
        }
    }
    
    printf("[vmm] Summary: Used: %zu KB, Free: %zu KB, Reserved: %zu KB\n",
           used * VMM_PAGE_SIZE / 1024,
           free * VMM_PAGE_SIZE / 1024,
           reserved * VMM_PAGE_SIZE / 1024);
}

void vmm_stats(size_t* used_kb, size_t* free_kb, size_t* reserved_kb) {
    size_t used = 0, free = 0, reserved = 0;
    
    for (size_t i = 0; i < VMM_MAX_PAGES; ) {
        if (page_table[i].alloc_pages > 0) {
            if (page_table[i].state == VMM_PAGE_USED) used += page_table[i].alloc_pages;
            else if (page_table[i].state == VMM_PAGE_RESERVED) reserved += page_table[i].alloc_pages;
            else free += page_table[i].alloc_pages;
            i += page_table[i].alloc_pages;
        } else {
            if (page_table[i].state == VMM_PAGE_USED) used++;
            else if (page_table[i].state == VMM_PAGE_RESERVED) reserved++;
            else free++;
            i++;
        }
    }
    
    if (used_kb) *used_kb = used * VMM_PAGE_SIZE / 1024;
    if (free_kb) *free_kb = free * VMM_PAGE_SIZE / 1024;
    if (reserved_kb) *reserved_kb = reserved * VMM_PAGE_SIZE / 1024;
}

size_t vmm_get_total_memory(void) {
    return VMM_MAX_PAGES * VMM_PAGE_SIZE;
}

size_t vmm_get_largest_free_block(void) {
    size_t max_free = 0;
    size_t current_free = 0;
    
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].state == VMM_PAGE_FREE) {
            current_free++;
            if (current_free > max_free) {
                max_free = current_free;
            }
        } else {
            current_free = 0;
        }
    }
    
    return max_free * VMM_PAGE_SIZE;
}

void vmm_set_tag(void* addr, uint32_t tag) {
    if (!addr) return;
    
    uintptr_t target = (uintptr_t)addr;
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].base_addr <= target && 
            target < page_table[i].base_addr + page_table[i].size) {
            if (page_table[i].state == VMM_PAGE_USED) {
                page_table[i].tag = tag;
            }
            return;
        }
    }
}

uint32_t vmm_get_tag(void* addr) {
    if (!addr) return 0;
    
    uintptr_t target = (uintptr_t)addr;
    for (size_t i = 0; i < VMM_MAX_PAGES; ++i) {
        if (page_table[i].base_addr <= target && 
            target < page_table[i].base_addr + page_table[i].size) {
            return page_table[i].tag;
        }
    }
    return 0;
}

