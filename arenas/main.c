#include <stdio.h>

#include "arena.h"
#include "arena.c"

int main(void)
{
    mem_arena* perm_arena = arena_create(GiB(1), MiB(1));

    for (;;)
    {
        arena_push(perm_arena, MiB(16), false);
        getc(stdin);
    }

    arena_destroy(perm_arena);
    return 0;
}
