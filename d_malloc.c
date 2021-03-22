#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_D_CONTROLS 1024
// sizeof(Node)が88
#define D_MEMORY_AVERAGE_SIZE 100
#define D_MEMORY_SIZE NUM_D_CONTROLS *D_MEMORY_AVERAGE_SIZE

char temp[1024];

typedef struct _d_control
{
    void *memory;
    size_t size;
    int8_t is_available; // 初期値0。freeしたら1。再割り当てしたら0。
} d_control_t;

// 割り当てたメモリ領域を管理する配列。
d_control_t d_controls[NUM_D_CONTROLS];
d_control_t *d_controls_head = d_controls;

// 割り当てるメモリ領域。
int8_t d_memory[D_MEMORY_SIZE];
int8_t *d_memory_head = d_memory;

// 配列で指定したアドレスをdmallocとdfreeしたときをデバッグする。
void *check_memories[] = {(void *)0x00000001403cb2e4};
size_t check_memories_size = sizeof(check_memories) / sizeof(void *);

// 配列で指定したアドレスをデバッグするかのフラグ。
int8_t enable_check_memories = 0;

void check_memory(void *memory, const char *from)
{
    if (!enable_check_memories)
    {
        return;
    }

    for (size_t i = 0; i < check_memories_size; i++)
    {
        void *cm = check_memories[i];
        if (memory == cm)
        {
            fprintf(stderr, "memory debug: %p at check_memory from %s\n", memory, from);
            return;
        }
    }
}

d_control_t *find_available_control(size_t size)
{
    // sizeに一致するis_availableなメモリ領域(d_control_t)を探す
    d_control_t *ptr;
    for (ptr = d_controls; ptr < d_controls_head; ptr++)
    {
        if (ptr->size == size && ptr->is_available)
        {
            return ptr;
        }
    }
    return NULL;
}
_Noreturn void dmalloc_exit(const char *msg)
{
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}
void *dmalloc(size_t size)
{
    d_control_t *cntrl = find_available_control(size);
    if (cntrl)
    {
        cntrl->is_available = 0;

        check_memory(cntrl->memory, "dmalloc");
        return cntrl->memory;
    }

    int8_t *d_memory_return = d_memory_head;
    d_memory_head += size;

    if (d_memory_head > d_memory + D_MEMORY_SIZE)
    {
        dmalloc_exit("d_memory limit exceeded at dmalloc");
    }

    cntrl = d_controls_head;
    cntrl->memory = (void *)d_memory_return;
    cntrl->is_available = 0;
    cntrl->size = size;
    d_controls_head++;

    if (d_controls_head > d_controls + NUM_D_CONTROLS)
    {
        dmalloc_exit("d_controls limit exceeded at dmalloc");
    }

    check_memory(d_memory_return, "dmalloc");

    return (void *)d_memory_return;
}
void dfree(void *_memory)
{
    check_memory(_memory, "dfree");

    d_control_t *ptr;
    for (ptr = d_controls; ptr < d_controls_head; ptr++)
    {
        if (ptr->memory == _memory)
        {
            if (ptr->is_available)
            {
                sprintf(temp, "memory %p is available at dfree\n", _memory);
                dmalloc_exit(temp);
            }
            ptr->is_available = 1;
            return;
        }
    }

    sprintf(temp, "memory %p is not dmalloc-ed at dfree\n", _memory);
    dmalloc_exit(temp);
}
void print_d_malloc_stat()
{
    size_t num_mallocs = 0;
    size_t num_available_mallocs = 0;

    d_control_t *ptr;
    for (ptr = d_controls; ptr < d_controls_head; ptr++)
    {
        if (ptr->is_available)
        {
            num_available_mallocs++;
        }
        else
        {
            num_mallocs++;
        }
    }
    printf("controls stat: (%zu + available %zu) / total %zu\n", num_mallocs, num_available_mallocs, num_mallocs + num_available_mallocs);
    printf("memory size: %zu / %u\n", d_memory_head - d_memory, D_MEMORY_SIZE);
}
void print_d_malloc_controls()
{
    uint32_t cnt = 0;
    d_control_t *ptr;
    for (ptr = d_controls; ptr < d_controls_head; ptr++)
    {
        cnt++;

        if (!ptr->is_available)
        {
            printf("(%04d) %p, %3zu, %d\n", cnt, ptr->memory, ptr->size, ptr->is_available);
        }
    }
}
