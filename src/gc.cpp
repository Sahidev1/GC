#include "gc.h"

using namespace GarbageCollector;

void Gc::mark(heap_chunk *chnk) { chnk->flags |= (0x1 << FLAG_MARK_BIT_INDEX); }
void Gc::unmark(heap_chunk *chnk) { chnk->flags &= ~(0x1 << FLAG_MARK_BIT_INDEX); }

int Gc::is_marked(heap_chunk *chnk) { return (chnk->flags >> FLAG_MARK_BIT_INDEX) & 0x1; }

heap_chunk *Gc::data_to_heap_chunk_addr(char *data_addr) {
    void *pot_ptr = *((void **)data_addr);
    if (pot_ptr == nullptr)
        return nullptr;

    heap_chunk *pot_chunk = (heap_chunk *)pot_ptr;

    return pot_chunk - 1;
}

/*
bool Gc::is_stack_reachable(heap_chunk *chnk) {

    char *stack_ptr = (char *)*(chnk->stack_ptr);
    char *chunk_ptr = (char *)chnk;

    return stack_ptr - sizeof(heap_chunk) == chunk_ptr;
}*/

// complexity time: O(n), memory; O(m)
std::vector<heap_chunk *> Gc::get_stack_reachable() {
    using namespace MemoryScanner;

    std::unique_ptr<StackIterator> scanIt = std::make_unique<StackIterator>(this->stackScanner->createIterator());
    std::vector<heap_chunk*> stack_reachable;

    this->stackScanner->scanNext(*scanIt);

    heap_chunk* pot_heap_addr;
    while(!scanIt->at_end){
        pot_heap_addr = data_to_heap_chunk_addr(reinterpret_cast<char*>(scanIt->curr));

        if(this->alloc_set.find(pot_heap_addr) != this->alloc_set.end()){
            stack_reachable.push_back(pot_heap_addr);
        }

        this->stackScanner->scanNext(*scanIt);
    }

    return stack_reachable;
}

// complexity O(n), where n is number of possible pointer data segments in heapchunk data block
int Gc::mark_from_chunk(heap_chunk *chunk, std::vector<heap_chunk *> &reach_able) {
    unsigned int seg_size = sizeof(void *);
    uint64_t seg_count = chunk->size / seg_size;
    char *seg = (char *)(chunk + 1);
    char *last_seg_sentinel = seg + seg_count * seg_size;

    heap_chunk *pot_heap_chunk;
    while (seg < last_seg_sentinel) {
        pot_heap_chunk = data_to_heap_chunk_addr(seg);
        seg += seg_size;
        if (pot_heap_chunk == nullptr)
            continue;

        auto it = alloc_set.find(pot_heap_chunk);
        if (it != alloc_set.end() && !is_marked(*it)) {
            mark(*it);
            reach_able.push_back(*it);
        }
    }

    return 0;
}

// complexity O(n)*O(K), where K is average number of potential pointer data segments in each heap chunk data block
int Gc::mark_phase() {
    auto reachable = get_stack_reachable(); // O(n) call, n is heapchunk vector element count

    heap_chunk *chunk;
    size_t size;
    while ((size = reachable.size()) > 0) {
        chunk = reachable[size - 1];
        reachable.pop_back();

        // mark(chunk);
        mark_from_chunk(
            chunk, reachable); // O(k) call, where k number of potential pointer data segments in the chunk data field
    }

    return 0;
}

/**
 * **stack_addr is a pointer to stack address which contains pointer to be set to allocated heap block.
 */
int Gc::internal_allocate(char **stack_addr, size_t bytes) {
    heap_chunk *chnk;

    if (this->allocator.zero_alloc((char **)&chnk, sizeof(heap_chunk) + bytes) != 0)
        return ALLOCATION_FAIL;

    chnk->stack_ptr = (char **)stack_addr;
    chnk->size = bytes;

    // setting allocation pointer to addr of data section of allocated heap block.
    *stack_addr = (char *)(chnk + 1);

    this->alloc_count++;

    this->alloc_vector.push_back(chnk);
    this->alloc_set.insert(chnk);

    return 0;
}

int Gc::man_free(char *heap_addr) {
    char **stck_ptr = &heap_addr;

    heap_chunk *chunk = reinterpret_cast<heap_chunk *>(heap_addr);
    chunk--;
    auto it = this->alloc_set.find(chunk);
    assert(it != this->alloc_set.end());
    if (it != this->alloc_set.end()) {
        for (auto i = this->alloc_vector.begin(); i < this->alloc_vector.end(); i++) {
            if (*i == *it) {
                this->alloc_vector.erase(i);
                this->alloc_set.erase(it);
                this->allocator.deallocate(reinterpret_cast<char **>(&chunk));
                this->alloc_count--;
                break;
            }
        }
    }

    return 0;
}

int Gc::sweep() {

    auto it = this->alloc_vector.begin();

    heap_chunk *chunk;
    while (it < this->alloc_vector.end()) {
        chunk = *it;

        if (!this->is_marked(chunk)) {
            it = this->alloc_vector.erase(it);

            auto sit = this->alloc_set.find(chunk);
            if (sit != this->alloc_set.end()) {
                this->alloc_set.erase(sit);
            }

            this->allocator.deallocate(reinterpret_cast<char **>(&chunk));
            this->alloc_count--;
            continue;
        }
        this->unmark(chunk);
        it++;
    }

    return 0;
}

Gc::Gc() {
    this->alloc_count = 0;
    this->stackScanner = std::make_unique<MemoryScanner::StackScanner>(new MemoryScanner::StackScanner());
}

Gc::Gc(pthread_t pthread_id){
    this->alloc_count = 0;
    this->stackScanner = std::make_unique<MemoryScanner::StackScanner>(new MemoryScanner::StackScanner(pthread_id));
}

Gc::~Gc() {};

int Gc::gc_run() {
    if (mark_phase() != 0)
        return GC_MARKPHASE_FAIL;
    if (sweep() != 0)
        return GC_SWEEP_FAIL;

    return 0;
};

/*___________________________________________________DEBUG METHODS___________________________________ */

#ifdef DEBUG
std::vector<heap_chunk *> Gc::getAllocVector() { return this->alloc_vector; }
#endif