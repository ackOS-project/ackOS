#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel/mm/alloc.h"
#include "kernel/mm/mem.h"
#include "kernel/mm/phys.h"
#include "kernel/lib/log.h"

static void allocate_phys_mem(void** memory, size_t* capacity, size_t min_size, uint32_t flags)
{
    const size_t aligned_size = ((min_size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    //void* addr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    virt_addr_t virt_addr = mem_allocate_and_map(get_kernel_vmm_context(), aligned_size, flags | MAP_PAGE_WRITE);

    *memory = virt_addr;
    *capacity = aligned_size;
}

static void deallocate_phys_mem(void* memory, size_t size)
{
    if (!mem_deallocate_and_unmap(get_kernel_vmm_context(), memory, size))
    {
        kprintf(KERN_WARN "failed to deallocate and unmap heap memory");
    }
}

// page headers forming a doubly linked list
// when there is only one remaining block in a page and it is free,
// then it is safe to release the pages back to the system.
struct page_node
{
    struct page_node* prev;
    struct page_node* next;
};

struct block_node
{
    size_t size;
    size_t capacity;
    struct block_node* previous;
    uint8_t is_tail : 1;
};

// eg
// (page_node)(block_node)    (block_node)
// (page_node)(block_node)  (block_node) (block_node) 

inline static struct block_node* get_next_block(struct block_node* node)
{
    return (struct block_node*)((uintptr_t)node + sizeof(struct block_node) + node->capacity);
}

inline static const struct block_node* get_next_block_const(const struct block_node* node)
{
    return (const struct block_node*)((uintptr_t)node + sizeof(struct block_node) + node->capacity);
}

inline static void* get_block_data(struct block_node* node)
{
    return (void*)((uintptr_t)node + sizeof(struct block_node));
}

inline static struct block_node* get_data_block(void* data)
{
    return (uintptr_t)data < sizeof(struct block_node) ? NULL : (struct block_node*)((uintptr_t)data - sizeof(struct block_node));
}

inline static struct block_node* get_page_head_block(struct page_node* node)
{
    return (struct block_node*)((uintptr_t)node + sizeof(struct page_node));
}

inline static const struct block_node* get_page_head_block_const(const struct page_node* node)
{
    return (const struct block_node*)((uintptr_t)node + sizeof(struct page_node));
}

inline static struct page_node* get_head_block_page(struct block_node* head_block)
{
    return (struct page_node*)((uintptr_t)head_block - sizeof(struct page_node));
}

static struct page_node* allocate_page_for_block_of(size_t size, uint32_t flags)
{
    size_t capacity = 0;
    void* memory = NULL;

    allocate_phys_mem(&memory, &capacity, sizeof(struct page_node) + sizeof(struct block_node) + size, flags);

    if(!memory) return NULL;

    struct page_node* page = (struct page_node*)memory;
    page->prev = NULL;
    page->next = NULL;

    struct block_node* head_block = get_page_head_block(page);

    head_block->size = size;
    head_block->capacity = capacity - sizeof(struct block_node) - sizeof(struct page_node);
    head_block->previous = NULL;
    head_block->is_tail = true;

    return page;
}

static struct block_node* allocate_block(struct page_node** head_page, size_t size, uint32_t flags)
{
    struct page_node* current_page = *head_page;

    while(current_page)
    {
        struct block_node* head_block = get_page_head_block(current_page);

        for (struct block_node* node = head_block; ; node = get_next_block(node))
        {
            size_t usable_space = node->capacity - node->size;

            if (usable_space >= size)
            {
                if (node->size == 0)
                {
                    node->size = size;

                    return node;
                }
                else if (usable_space >= (sizeof(struct block_node) + size))
                {
                    node->capacity = node->size;

                    struct block_node* new_node = get_next_block(node);

                    new_node->size = size;
                    new_node->capacity = usable_space - sizeof(struct block_node);
                    new_node->previous = node;
                    new_node->is_tail = node->is_tail;
                    node->is_tail = false;

                    return new_node;
                }
            }

            if (node->is_tail) break;
        }

        current_page = current_page->next;
    }

    struct page_node* new_page = allocate_page_for_block_of(size, flags);

    if(!new_page) return NULL;

    if(*head_page) (*head_page)->prev = new_page;
    new_page->next = *head_page;
    *head_page = new_page;

    return get_page_head_block(new_page);
}

static void deallocate_block(struct page_node** head_page, struct block_node* node)
{
    if (node->size == 0 || node->capacity == 0)
    {
        kprintf(KERN_PANIC "DOUBLE FREE or CORRUPTION in heap allocator!");

        return;
    }

    struct block_node* product_node = node;

    node->size = 0;

    if (!node->is_tail)
    {
        struct block_node* next_node = get_next_block(node);

        if (next_node->size == 0)
        {
            if (!next_node->is_tail)
            {
                struct block_node* next_next_node = get_next_block(next_node);

                next_next_node->previous = node;
            }

            node->capacity += sizeof(struct block_node) + next_node->capacity;
            node->is_tail = next_node->is_tail;
        }
    }

    if (node->previous)
    {
        struct block_node* previous_node = node->previous;

        if (!node->is_tail)
        {
            struct block_node* next_node = get_next_block(node);

            next_node->previous = previous_node;
        }

        previous_node->is_tail = node->is_tail;
        previous_node->capacity += sizeof(struct block_node) + node->capacity;

        product_node = previous_node;
    }

    if (!product_node->previous && product_node->is_tail && product_node->size == 0)
    {
        struct page_node* page = get_head_block_page(product_node);
        size_t size = sizeof(struct page_node) + sizeof(struct block_node) + product_node->capacity;

        if(page->prev) page->prev->next = page->next;
        if(page->next) page->next->prev = page->prev;
        if(*head_page == page) *head_page = page->next;

        deallocate_phys_mem((void*)page, size);
    }
}

static struct page_node* heap_pages = NULL;

void* kmalloc(size_t size, uint32_t flags)
{
    struct block_node* new_block = allocate_block(&heap_pages, size, flags);

    return get_block_data(new_block);
}

void kfree(void* mem)
{
    if (!mem) return;

    deallocate_block(&heap_pages, get_data_block(mem));
}
