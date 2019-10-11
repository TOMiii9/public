#ifndef BLOCK_ALLOC_H_
#define BLOCK_ALLOC_H_

#include "util.h"
#include "typedefs.h"
#include <Windows.h>

/*
===============================================================================



         Helper classes to avoid dynamic memory allocations during gameplay.


     block_allocator: allocates a huge slab of memory and divides into 'Item_Type'
                      sized chunks. The program terminates when the block is full.
                      Might implement allocation of additional blocks later.

     linear_allocator: allocates a huge slab of memory and allocates variable length
                       chunks within this block. It calls the constructors but the
                       destructors never get called. The frame_allocator is a
                       linear_allocator.

===============================================================================
*/

/*

blocks must allocate from a linear_allocator

*/

#define round_up(num_to_round, multiple) (((num_to_round + multiple - 1) / multiple) * multiple)

struct Linear_Allocator;

template <typename Item_Type, i32 block_size>
struct Block_Allocator {
    Item_Type *       buffer;
    Item_Type *       free_list[block_size];
    i32               index;
    i8 *              allocator_name;
    Linear_Allocator *linear_allocator;

    void set_name(i8 *str) {
        allocator_name = str;
    }

    void init(Linear_Allocator *allocator, const i8 *name) {
        set_name((i8 *)name);
        linear_allocator = allocator;
        buffer           = (Item_Type *)linear_allocator->alloc(sizeof(Item_Type) * block_size);
        index            = 0;
        for (i32 i = 0; i < block_size; i++) {
            free_list[i] = &buffer[i];
        }
    }

    bool       full();
    bool       empty();
    void       clear();
    Item_Type *alloc();
    void       free(Item_Type *object);
};

template <typename Item_Type, i32 block_size>
bool Block_Allocator<Item_Type, block_size>::full() {
    return index >= block_size;
}

template <typename Item_Type, i32 block_size>
bool Block_Allocator<Item_Type, block_size>::empty() {
    return index = 0;
}

template <typename Item_Type, i32 block_size>
Item_Type *Block_Allocator<Item_Type, block_size>::alloc() {
    if (full()) {
        panic(stringf("'%s' block_allocator is full", allocator_name));
        return NULL;
    }

    Item_Type *memory_slot = new (free_list[index]) Item_Type();

    // move to the next slot
    index++;

    return memory_slot;
}

template <typename Item_Type, i32 block_size>
void Block_Allocator<Item_Type, block_size>::free(Item_Type *object) {
    object->~Item_Type();
    index--;
    free_list[index] = object;
}

template <typename Item_Type, i32 block_size>
void Block_Allocator<Item_Type, block_size>::clear() {
}

struct Linear_Allocator {
    u8 *              arena;
    u8 *              start;
    i32               arena_size;
    i32               allocated_bytes;
    i8 *              allocator_name;
    Linear_Allocator *parent_allocator;

    void init(i32 size, Linear_Allocator *parent, const i8 *name) {
        parent_allocator = parent;
        allocator_name   = (i8 *)name;
        arena_size       = size;
        allocated_bytes  = 0;

        if (parent_allocator == NULL) {
            // this is the global_arena
            arena = new u8[arena_size];
        } else {
            arena = (u8 *)parent_allocator->alloc(size);
        }

        // to debug misalligned allocations
        // arena = arena + 1;

        start = arena;
        std::memset(arena, 0, arena_size);
    }

    void *alloc(i32 bytes) {
        u8 *result = start;

        allocated_bytes += bytes;
        start = start + bytes;
        if (allocated_bytes > arena_size) {
            panic(stringf("%s failed to allocate %d bytes", allocator_name, bytes));
        }
        return result;
    }

    void reset() {
        start           = arena;
        allocated_bytes = 0;
    }
};

struct Memory_Manager {
    Linear_Allocator global_arena;
    Linear_Allocator per_frame_allocator;

    void init_allocators();
};

Memory_Manager *get_memory_manager();

//
// Memory will be deallocated in the next frame. The frame allocator doesnt call the destructors.
//
template <typename Item_Type>
Item_Type *create_temporary_object(const i32 num_objects) {
    constexpr i32  alignment = 16;
    i32            size      = (sizeof(Item_Type) * num_objects) + alignment - 1;
    std::ptrdiff_t p         = std::ptrdiff_t(get_memory_manager()->per_frame_allocator.alloc(size));
    p                        = round_up(p, 16);

    if (((uintptr_t)p % 16) != 0) {
        panic("misalligned memory allocation");
    }

    Item_Type *obj = new ((void *)p) Item_Type[num_objects];
    return obj;
}

template <typename Item_Type>
Item_Type *create_object(i32 num_objects) {
    constexpr i32  alignment = 16;
    i32            size      = (sizeof(Item_Type) * num_objects) + alignment - 1;
    std::ptrdiff_t p         = std::ptrdiff_t(get_memory_manager()->global_arena.alloc(size));
    p                        = round_up(p, 16);

    if (((uintptr_t)p % 16) != 0) {
        panic("misalligned memory allocation");
    }

    Item_Type *obj = new ((void *)p) Item_Type[num_objects];
    return obj;
}

template <typename Item_Type>
Item_Type *create_object() {
    Item_Type *obj = create_object<Item_Type>(1);
    return obj;
}

#endif
