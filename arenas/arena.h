#ifndef ARENA
#define ARENA

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define KiB(n) ((uint64_t)(n) << 10)
#define MiB(n) ((uint64_t)(n) << 20)
#define GiB(n) ((uint64_t)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (b) : (a))
#define ALIGN_UP_POW2(n,p) (((uint64_t)(n) + ((uint64_t)(p) - 1)) & (~((uint64_t)(p) - 1)))

#define ARENA_BASE_POS (sizeof(mem_arena))
#define ARENA_ALIGN (sizeof(void*)) //size of void pointer gives size computer word size

typedef struct
{
    uint64_t reserve_size;
    uint64_t commit_size;

    uint64_t position;
    uint64_t commit_position;
} mem_arena;

mem_arena* arena_create(uint64_t reserve_size, uint64_t commit_size);
void arena_destroy(mem_arena* arena);
void* arena_push(mem_arena* arena, uint64_t size, int8_t non_zero);
void arena_pop(mem_arena* arena, uint64_t size);
void arena_pop_to(mem_arena* arena, uint64_t position);
void arena_clear(mem_arena* arena);

#define PUSH_STRUCT(arena, T) (T*)arena_push((arena), sizeof(T), false)
#define PUSH_STRUCT_NZ(arena, T) (T*)arena_push((arena), sizeof(T), true)
#define PUSH_ARRAY(arena, T, n) (T*)arena_push((arena), sizeof(T) * (n), false)
#define PUSH_ARRAY_NZ(arena, T, n) (T*)arena_push((arena), sizeof(T) * (n), true)

uint32_t plat_get_pagesize(void);

void* plat_mem_reserve(uint64_t size);
int32_t plat_mem_commit(void* ptr, uint64_t size);
int32_t plat_mem_decommit(void* ptr, uint64_t size);
int32_t plat_mem_release(void* ptr, uint64_t size);

#endif
