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

//#ifndef DEBUG
//#define DEBUG
//#endif


#ifdef DEBUG
#include "../debug_tools/debugger.h"
#include <iomanip>
#endif 

enum err_codes { ALLOCATION_FAIL = 1, GC_MARKPHASE_FAIL, GC_SWEEP_FAIL };


//allocated heap chunk memory layout starting from the left as lowest memory address: [[struct heap_chunk]|[data segment]]
//the heap chunk struct size field allows as to access data segment within allocated range. 
/**
 * Heap chunk struct.
 * To access Data Segment go to mem addr: &chunk + sizeof(heap_chunk).
 * Valid Data Segment Mem range for a chunk: [&chunk + sizeof(heap_chunk), &chunk + sizeof(heap_chunk) + size]
 */
struct heap_chunk {
    size_t size;    /** Size of data segment of chunk in bytes */
    uint32_t flags; /** Holds flag bits for the heapchunk such as mark status */
};

// defining << operator for heap_chunks
std::ostream& operator<<(std::ostream& os, const heap_chunk& h);


namespace GarbageCollector {
class Gc {
  private:
    std::vector<heap_chunk *> alloc_vector;
    std::unordered_set<heap_chunk *> alloc_set;
    Allocator<char> allocator;
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

    /**
     * Constructor for gc instance on current running thread
     */
    Gc();
    /**
     * Constuctor for gc instance running on a specific pthread
     */
    Gc(pthread_t pthread_id);
    virtual ~Gc();

    /**
     * Allocates memory for certain numbers of an element
     * @tparam U Type of element to allocate for
     * @param ptr_addr The address of the heap pointer to allocate memory to. 
     * @param elem_cnt The number of elements to allocate for. 
     */
    template <typename U> int allocate(U** ptr_addr, unsigned long elem_cnt) {
        return internal_allocate(reinterpret_cast<char **>(ptr_addr), sizeof(U) * elem_cnt);
    };

        
    /**
     * Hard frees a allocation, very expensive operation, not recommended. 
     */
    template <typename U> void hard_free(U *heap_addr) {
         man_hard_free(reinterpret_cast<char *>(heap_addr));
    };

    /**
     * Runs the garbage collector. 
     */
    int run();

#ifdef DEBUG
    std::vector<heap_chunk *> getAllocVector();
    void stack_dump();
#endif
};

}; // namespace GarbageCollector

#endif