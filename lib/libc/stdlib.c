#include <liback/util.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

/*
// a static block-based heap
#define HEAP_SIZE 10 KiB

static uint8_t heap_memory[HEAP_SIZE];
struct block_node* heap_head_block = NULL;

struct block_node
{
    size_t size;
    size_t capacity;
    struct block_node* previous;
    uint8_t is_tail : 1;
};

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

inline static struct block_node* data_to_block_node(void* data)
{
    return (struct block_node*)((uintptr_t)data - sizeof(struct block_node));
}

static void create_heap_if_not_already(void)
{
    if(heap_head_block) return;

    heap_head_block = (struct block_node*)heap_memory;

    heap_head_block->size = 0;
    heap_head_block->capacity = HEAP_SIZE - sizeof(struct block_node);
    heap_head_block->previous = NULL;
    heap_head_block->is_tail = true;
}

static struct block_node* allocate_block(struct block_node* head_block, size_t size)
{
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

    return NULL;
}

static void deallocate_block(struct block_node* node)
{
    if (node->size == 0 || node->capacity == 0)
    {
        // puts("DOUBLE FREE or CORRUPTION!");

        return;
    }

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
    }
}

static struct block_node* reallocate_block(struct block_node* head_block, struct block_node* node, size_t size)
{
    if (size <= node->capacity)
    {
        node->size = size;

        return node;
    }

    struct block_node* new_node = allocate_block(head_block, size);

    if (!new_node) return NULL;

    memcpy(get_block_data(new_node), get_block_data(node), size);

    deallocate_block(node);

    return new_node;
}

void* malloc(size_t size)
{
    create_heap_if_not_already();

    struct block_node* node = allocate_block(heap_head_block, size);

    if(!node) return NULL;

    return get_block_data(node);
}

void* realloc(void* mem, size_t new_size)
{
    struct block_node* node = reallocate_block(heap_head_block, data_to_block_node(mem), new_size);

    if(!node) return NULL;

    return get_block_data(node);
}

void free(void* ptr)
{
    deallocate_block(data_to_block_node(ptr));
}

*/

//////////////////
static const char strtoint_lookup_table[] = "0123456789abcdefghijklmnopqrstuvwxyz";

static int strtoint_lut_binary_search(char d, int low, int high)
{
    if(low > high)
    {
        return -1;
    }
    
    int mid = low + (high - low) / 2;

    if(strtoint_lookup_table[mid] == d)
    {
        return mid;
    }
    else if(strtoint_lookup_table[mid] > d)
    {
        return strtoint_lut_binary_search(d, low, mid - 1);
    }
        
    return strtoint_lut_binary_search(d, mid + 1, high); 
}

static int strtoint_lut_find(char d, int base)
{
    return strtoint_lut_binary_search(tolower(d), 0, base - 1);
}

long int strtol(const char* str, char** endptr, int base)
{
    long int res = 0;
    size_t len = strlen(str);
    char* endp = NULL;

    if(endptr == NULL)
    {
        endptr = &endp;
    }

    *endptr = (char*)str;

    if(len == 0)
    {
        return 0;
    }

    if(base < 1 || (size_t)base > (sizeof(strtoint_lookup_table) / sizeof(char)))
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], base);

        if(d < 0) break;

        res = (res * base) + d;

        (*endptr)++;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

/*
enable when a testing framework has been established

TEST test_strtol()
{
    char* endp = NULL;
    const char* str = "1234abc";
    CHECK(strtol(str, &endp, 10) == 1234);
    CHECK(endp[0] == str[4]);
}

TEST test_strtol_neg_sign()
{
    char* endp = NULL;
    const char* str = "-1234";
    CHECK(strtol(str, &endp, 10) == -1234);
}

TEST test_strtol_pos_sign()
{
    char* endp = NULL;
    const char* str = "+1234";
    CHECK(strtol(str, &endp, 10) == 1234);
}

TEST test_strtol_null_endptr()
{
    const char* str = "1234";
    CHECK(strtol(str, NULL, 10) == 1234);
}

TEST test_strtol_hex()
{
    char* endp = NULL;
    const char* str = "FEDCBA09";
    CHECK(strtol(str, &endp, 16) == 0xFEDCBA09);
}

TEST test_strtol_termination()
{
    char* endp = NULL;
    const char* str = "123 241";
    CHECK(strtol(str, &endp, 16) == 123);
}
*/

unsigned long int strtoul(const char* str, char** endptr, int base)
{
    unsigned long int res = 0;
    size_t len = strlen(str);
    char* endp = NULL;

    if(endptr == NULL)
    {
        endptr = &endp;
    }

    *endptr = (char*)str;

    if(len == 0)
    {
        return 0;
    }

    if(base < 1 || (size_t)base > (sizeof(strtoint_lookup_table) / sizeof(char)))
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], base);

        if(d < 0) break;

        res = (res * base) + d;

        (*endptr)++;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

long int atol(const char* str)
{
    return strtol(str, NULL, 10);
}

int atoi(const char* str)
{
    unsigned long int res = 0;
    size_t len = strlen(str);

    if(len == 0)
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], 10);

        if(d < 0) break;

        res = (res * 10) + d;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

int abs(int i)
{
    return i < 0 ? -i : i;
}

