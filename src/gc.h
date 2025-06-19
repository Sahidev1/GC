#ifndef GC_H
#define GC_H

#include "allocator.h"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>

#define FLAG_MARK_BIT_INDEX 0

#ifndef DEBUG
#define DEBUG
#endif

enum err_codes {
    ALLOCATION_FAIL = 1,
    GC_MARKPHASE_FAIL,
    GC_SWEEP_FAIL
};

struct heap_chunk {
    char **stack_ptr;
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
//struct heap_chunk;
//template <typename T> class Allocator;

class Gc {
  private:
    std::vector<heap_chunk *> alloc_vector;
    std::unordered_set<heap_chunk *> alloc_set;
    Allocator<char> allocator;

    

    void mark(heap_chunk *chnk);
    int is_marked(heap_chunk *chnk);
    heap_chunk *data_to_heap_chunk_addr(char *data_addr);
    bool is_stack_reachable(heap_chunk *chnk);
    std::vector<heap_chunk *> get_stack_reachable();
    int mark_from_chunk(heap_chunk *chunk, std::vector<heap_chunk *> &reach_able);
    int mark_phase();
    int internal_allocate(char **stack_addr, size_t bytes);
    int sweep();

  public:
    uint32_t alloc_count;

    Gc();
    virtual ~Gc();
    template <typename U> int gc_allocate(U **stack_addr, unsigned long elem_cnt) {
        return internal_allocate(reinterpret_cast<char **>(stack_addr), sizeof(U) * elem_cnt);
    };

    int gc_run();

    #ifdef DEBUG
    std::vector<heap_chunk *> getAllocVector();
    #endif
};



#endif