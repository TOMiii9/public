
#include "memory.h"

#define LOG_MEM_ALLOC 0

void *operator new(std::size_t sz) {
#if LOG_MEM_ALLOC
    char tmp[256];
    std::sprintf(tmp, "[MEM] new called, size = %d\n", (int)sz);
    OutputDebugStringA(tmp);
#endif

    return std::malloc(sz);
}
void operator delete(void *ptr) noexcept {
#if LOG_MEM_ALLOC
    OutputDebugStringA("[MEM] delete called\n");
#endif
    std::free(ptr);
}

void *operator new[](std::size_t sz) {
#if LOG_MEM_ALLOC
    char tmp[256];
    std::sprintf(tmp, "[MEM] new[] called, size = %d\n", (int)sz);
    OutputDebugStringA(tmp);
#endif
    return std::malloc(sz);
}
void operator delete[](void *ptr) noexcept {
#if LOG_MEM_ALLOC
    OutputDebugStringA("[MEM] delete[] called\n");
#endif
    std::free(ptr);
}

Memory_Manager *get_memory_manager() {
    static Memory_Manager memoryman;
    return &memoryman;
}

void Memory_Manager::init_allocators() {
    global_arena.init(1024 * 1024 * 256, NULL, "global_arena");
    per_frame_allocator.init(1024 * 1024 * 8, &global_arena, "per_frame_allocator");
}
