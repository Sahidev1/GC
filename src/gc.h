#ifndef GC_H
#define GC_H

#include "allocator.h"
#include "stackscanner.h"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>
#include <memory>

#define FLAG_MARK_BIT_INDEX 0
#define FLAG_SOFT_DEALLOC_BIT_INDEX 1

#ifndef DEBUG
#define DEBUG
#endif

enum err_codes { ALLOCATION_FAIL = 1, GC_MARKPHASE_FAIL, GC_SWEEP_FAIL };

struct heap_chunk {
    //char **stack_ptr;
    size_t size;
    uint32_t flags;
};

#if INTPTR_MAX == INT64_MAX
#define PTR_SIZE 8
#elif INTPTR_MAX == INT32_MAX
#define PTR_SIZE 4
#else
#error "NON COMAPTIBLE POINTER SIZE"
#endif

// Forward declaration of heap_chunk and Allocator
// struct heap_chunk;
// template <typename T> class Allocator;

namespace GarbageCollector {
class Gc {
  private:
    std::vector<heap_chunk *> alloc_vector;
    std::unordered_set<heap_chunk *> alloc_set;
    Allocator<char> allocator;
    void *stack_base;
    size_t stack_size;
    std::unique_ptr<MemoryScanner::StackScanner> stackScanner;

    void mark(heap_chunk *chnk);
    void unmark(heap_chunk *chnk);
    void set_soft_dealloc_flag(heap_chunk *chnk);
    void clear_soft_dealloc_flag(heap_chunk *chnk);
    int is_marked(heap_chunk *chnk);
    heap_chunk *data_to_heap_chunk_addr(uintptr_t stack_data_seg);
    std::vector<heap_chunk *> get_stack_reachable();
    int mark_from_chunk(heap_chunk *chunk, std::vector<heap_chunk *> &reach_able);
    int mark_phase();
    int internal_allocate(char **stack_addr, size_t bytes);
    int sweep();
    int man_hard_free(char *heap_addr);

  public:
    uint32_t alloc_count;

    Gc();
    Gc(pthread_t pthread_id);
    virtual ~Gc();
    template <typename U> int allocate(U** ptr_stack_addr, unsigned long elem_cnt) {
        return internal_allocate(reinterpret_cast<char **>(ptr_stack_addr), sizeof(U) * elem_cnt);
    };

    

    template <typename U> void manual_free(U *heap_addr) {
         man_hard_free(reinterpret_cast<char *>(heap_addr));
    };

    int run();

#ifdef DEBUG
    std::vector<heap_chunk *> getAllocVector();
    void stack_dump();
#endif
};

}; // namespace GarbageCollector

#define GC_ALLOCATE(gc, ptr, elem_cnt) (gc)->gc_allocate(&ptr, elem_cnt)

#define GC_FULL_ALLOCATE(gc, type, expr, elem_cnt)                                                                     \
    type *expr;                                                                                                        \
    gc->allocate(&expr, elem_cnt);

#define GC_MAN_FREE(gc, ptr) (gc)->manual_free(ptr);

#define GC_RUN(gc) (gc)->gc_run();

#endif