#include <paging.h>
#include <pmm.h>
#include <string.h>

static page_directory_t *kernel_page_directory = NULL;
page_directory_t *current_page_directory = NULL;

static inline size_t pd_index(uintptr_t addr) {
    return (addr >> 22) & 0x3FF;
}

static inline size_t pt_index(uintptr_t addr) {
    return (addr >> 12) & 0x3FF;
}

// Retrieve or allocate a page table for a virtual address
static page_table_t *get_or_create_page_table(page_directory_t *pd, uintptr_t virt_addr, bool create) {
    size_t pdi = pd_index(virt_addr);
    page_entry_t entry = pd->entries[pdi];

    if (entry & PAGE_PRESENT) {
        return (page_table_t *)(entry & ~0xFFF);
    }

    if (!create) return NULL;

    void *new_table = pmm_alloc_page();
    if (!new_table) return NULL;

    memset(new_table, 0, PAGE_SIZE);
    pd->entries[pdi] = (uintptr_t)new_table | PAGE_PRESENT | PAGE_WRITABLE;

    return (page_table_t *)new_table;
}

void paging_init(void) {
    kernel_page_directory = (page_directory_t *)pmm_alloc_page();
    if (!kernel_page_directory) return;

    memset(kernel_page_directory, 0, PAGE_SIZE);
    current_page_directory = kernel_page_directory;

    // Optional: identity map lower memory or map kernel to higher half

    paging_enable();
}

bool paging_map_page(uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags) {
    if (!pmm_is_page_aligned(virt_addr) || !pmm_is_page_aligned(phys_addr))
        return false;

    page_table_t *pt = get_or_create_page_table(current_page_directory, virt_addr, true);
    if (!pt) return false;

    size_t pti = pt_index(virt_addr);
    pt->entries[pti] = phys_addr | (flags & 0xFFF) | PAGE_PRESENT;

    paging_invalidate(virt_addr);
    return true;
}

bool paging_unmap_page(uintptr_t virt_addr) {
    if (!pmm_is_page_aligned(virt_addr))
        return false;

    page_table_t *pt = get_or_create_page_table(current_page_directory, virt_addr, false);
    if (!pt) return false;

    size_t pti = pt_index(virt_addr);
    if (!(pt->entries[pti] & PAGE_PRESENT))
        return false;

    pt->entries[pti] = 0;
    paging_invalidate(virt_addr);

    return true;
}

uintptr_t paging_get_physical(uintptr_t virt_addr) {
    page_table_t *pt = get_or_create_page_table(current_page_directory, virt_addr, false);
    if (!pt) return 0;

    size_t pti = pt_index(virt_addr);
    page_entry_t entry = pt->entries[pti];
    if (!(entry & PAGE_PRESENT)) return 0;

    return entry & ~0xFFF;
}

void paging_invalidate(uintptr_t virt_addr) {
    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");
}

void paging_enable(void) {
    uintptr_t pd_phys = (uintptr_t)current_page_directory;

    asm volatile("mov %0, %%cr3" :: "r"(pd_phys));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
