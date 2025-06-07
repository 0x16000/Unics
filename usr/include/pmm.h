#ifndef PMM_H
#define PMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PMM_PAGE_SIZE 4096
#define PMM_MAX_PAGES 32768  // ~128MB physical memory (32768 * 4096)

void pmm_init(void);
void* pmm_alloc_page(void);
void pmm_free_page(void* addr);
void pmm_dump_state(void);

#endif // PMM_H
