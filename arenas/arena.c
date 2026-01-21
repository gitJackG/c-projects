#include "arena.h"

mem_arena* arena_create(uint64_t reserve_size, uint64_t commit_size)
{
    uint32_t pagesize = plat_get_pagesize();

    reserve_size = ALIGN_UP_POW2(reserve_size, pagesize);
    commit_size = ALIGN_UP_POW2(commit_size, pagesize);

    mem_arena* arena = plat_mem_reserve(reserve_size);

    if (!plat_mem_commit(arena, commit_size))
    {
        return NULL;
    }

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->position = ARENA_BASE_POS;
    arena->commit_position = commit_size;

    return arena;
}

void arena_destroy(mem_arena* arena)
{
    plat_mem_release(arena, arena->reserve_size);
}

void* arena_push(mem_arena* arena, uint64_t size, int8_t non_zero)
{
    uint64_t pos_aligned = ALIGN_UP_POW2(arena->position, ARENA_ALIGN);
    uint64_t new_pos = pos_aligned + size;

    if (new_pos > arena->reserve_size)
    {
        return NULL;
    }

    if (new_pos > arena->commit_position)
    {
        uint64_t new_commit_pos = new_pos;
        new_commit_pos += arena->commit_size - 1;
        new_commit_pos -= new_commit_pos % arena->commit_size;
        new_commit_pos = MIN(new_commit_pos, arena->reserve_size);

        uint8_t* mem = (uint8_t*)arena + arena->commit_position;
        uint64_t commit_size = new_commit_pos - arena->commit_position;

        if (!plat_mem_commit(mem, commit_size))
        {
            return NULL;
        }

        arena->commit_position = new_commit_pos;
    }

    arena->position = new_pos;

    uint8_t* out = (uint8_t*)arena + pos_aligned;

    if (!non_zero)
    {
        memset(out, 0, size);
    }

    return out;
}

void arena_pop(mem_arena* arena, uint64_t size)
{
    size = MIN(size, arena->position - ARENA_BASE_POS);
    arena->position -= size;
}

void arena_pop_to(mem_arena* arena, uint64_t position)
{
    uint64_t size = position < arena->position ? arena->position - position : 0;
    arena_pop(arena, size);
}

void arena_clear(mem_arena* arena)
{
    arena_pop_to(arena, ARENA_BASE_POS);
}

#ifdef _WIN32

#include <windows.h>

uint32_t plat_get_pagesize(void)
{
    SYSTEM_INFO sysinfo = { 0 };
    GetSystemInfo(&sysinfo);

    return sysinfo.dwPageSize;
}

void* plat_mem_reserve(uint64_t size)
{
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

int32_t plat_mem_commit(void* ptr, uint64_t size)
{
    void* ret = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return ret != NULL;
}

int32_t plat_mem_decommit(void* ptr, uint64_t size)
{
    return VirtualFree(ptr, size, MEM_DECOMMIT);
}

int32_t plat_mem_release(void* ptr, uint64_t size)
{
    return VirtualFree(ptr, size, MEM_RELEASE);
}

#endif
