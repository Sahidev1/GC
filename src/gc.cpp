#include "gc.h"

using namespace GarbageCollector;

#define SET_BIT_INDEX(val, bit_index) (val |= (0x1 << bit_index))
#define CLEAR_BIT_INDEX(val, bit_index) (val &= ~(0x1 << bit_index))

void Gc::mark(heap_chunk *chnk) { chnk->flags |= (0x1 << FLAG_MARK_BIT_INDEX); }
void Gc::unmark(heap_chunk *chnk) { chnk->flags &= ~(0x1 << FLAG_MARK_BIT_INDEX); }

void Gc::set_soft_dealloc_flag(heap_chunk *chnk) { SET_BIT_INDEX(chnk->flags, FLAG_SOFT_DEALLOC_BIT_INDEX); }
void Gc::clear_soft_dealloc_flag(heap_chunk *chnk) { CLEAR_BIT_INDEX(chnk->flags, FLAG_SOFT_DEALLOC_BIT_INDEX); }

int Gc::is_marked(heap_chunk *chnk) { return (chnk->flags >> FLAG_MARK_BIT_INDEX) & 0x1; }

heap_chunk *Gc::data_to_heap_chunk_addr(uintptr_t stack_data_seg) {
    if (stack_data_seg == 0)
        return nullptr;

    heap_chunk *pot_chunk = reinterpret_cast<heap_chunk *>(stack_data_seg);
    return pot_chunk - 1;
}

// complexity time: O(n), memory; O(m)
std::vector<heap_chunk *> Gc::get_stack_reachable() {
    using namespace MemoryScanner;

    uintptr_t scan_ref;
    std::unique_ptr<MemoryScanner::StackIterator> scanIt = this->stackScanner->createIterator(&scan_ref);
    std::vector<heap_chunk *> stack_reachable;

    this->stackScanner->scanNext(*scanIt);

    heap_chunk *pot_heap_addr;
    while (!scanIt->at_end) {
        pot_heap_addr = data_to_heap_chunk_addr(scanIt->curr);

        if (this->alloc_set.find(pot_heap_addr) != this->alloc_set.end()) {
            mark(pot_heap_addr);
            stack_reachable.push_back(pot_heap_addr);
        }

        this->stackScanner->scanNext(*scanIt);
    }

    return stack_reachable;
}

// complexity O(n), where n is number of possible pointer data segments in heapchunk data block
int Gc::mark_from_chunk(heap_chunk *chunk, std::vector<heap_chunk *> &reach_able) {
    size_t seg_size = sizeof(void *);                  // size of mem segments that can hold ptrs
    size_t seg_count = chunk->size / seg_size;         // number of segments
    void **seg = reinterpret_cast<void **>(chunk + 1); // points to the start of the data segment
    void **last_seg_sentinel = seg + seg_count;        // sentinel to check for end of segment

    heap_chunk *pot_heap_chunk;
    while (seg < last_seg_sentinel) {
        // casting dereferenced segment ptr to a heap_chunk ptr, we take minus one because we assume
        // any heap ptr in data segment will point to start of heap chunk data segment.
        pot_heap_chunk = reinterpret_cast<heap_chunk *>(*seg) - 1;
    
        seg = (void **)(reinterpret_cast<char *>(seg) + seg_size); //increment segment pointer

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
    size_t sizae = reachable.size();

    heap_chunk *chunk;
    size_t size;
    while ((size = reachable.size()) > 0) {
        chunk = reachable[size - 1];
        reachable.pop_back();
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
    *stack_addr = this->allocator.zero_seg_alloc(sizeof(heap_chunk) + bytes);

    if (*stack_addr == nullptr)
        return ALLOCATION_FAIL;

    chnk = reinterpret_cast<heap_chunk *>(*stack_addr);
    chnk->size = bytes;

    *stack_addr += sizeof(heap_chunk);

    this->alloc_count++;
    this->alloc_vector.push_back(chnk);
    this->alloc_set.insert(chnk);
    chnk = nullptr;

    return 0;
}

int Gc::man_hard_free(char *heap_addr) {
    heap_chunk *chunk = reinterpret_cast<heap_chunk *>(heap_addr);
    chunk--;
    auto it = this->alloc_set.find(chunk);

    if (it != this->alloc_set.end()) {
        for (auto i = this->alloc_vector.begin(); i < this->alloc_vector.end(); i++) {
            if (*i == *it) {
                this->alloc_vector.erase(i);
                this->alloc_set.erase(it);
                this->allocator.deallocate(heap_addr);
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
    this->stackScanner = std::make_unique<MemoryScanner::StackScanner>();
}

Gc::Gc(pthread_t pthread_id) {
    this->alloc_count = 0;
    this->stackScanner = std::make_unique<MemoryScanner::StackScanner>(pthread_id);
}

Gc::~Gc() {
    heap_chunk *tmp;
    for (auto it = this->alloc_vector.begin(); it < this->alloc_vector.end();) {
        tmp = *it;
        this->alloc_set.erase(tmp);
        it = this->alloc_vector.erase(it);
        this->allocator.deallocate(reinterpret_cast<char *>(tmp));
    }
};

int Gc::run() {
    if (mark_phase() != 0)
        return GC_MARKPHASE_FAIL;
    if (sweep() != 0)
        return GC_SWEEP_FAIL;
    return 0;
};

/*___________________________________________________DEBUG METHODS___________________________________ */

#ifdef DEBUG
std::vector<heap_chunk *> Gc::getAllocVector() { return this->alloc_vector; }

std::ostream &operator<<(std::ostream &os, const heap_chunk &h) {
    std::ios::fmtflags f(os.flags());
    char fill_char = os.fill();

    os << "heap_chunk{\n\tflags= " << h.flags << "\n\tsize= " << h.size << "\n\t" << "DATA SECTION: " << "\n\t";

    heap_chunk const *hptr = &h;
    char const *ptr = reinterpret_cast<char const *>(hptr) + sizeof(heap_chunk);

    for (int i = 0; i < h.size; ++i) {
        os << std::hex << std::setw(2) << std::setfill('0')
           << (static_cast<unsigned int>(static_cast<unsigned char>(ptr[i])));

        if ((i + 1) % sizeof(intptr_t) == 0)
            os << "\n\t";
        else
            os << " ";
    }

    os.flags(f);
    os.fill(fill_char);

    os << "\n}" << std::endl;
    return os;
}

// VERY DODGY DO NOT USE
void Gc::stack_dump() {
    void *stackBase = this->stackScanner->getStackAddr();
    void *stackEnd = VOID_PTR_ADD(stackBase, this->stackScanner->getStackSize());
    void *curr = stackBase;

    std::cout << "\n______________STACKPRINT_________________\n";
    while (curr < stackEnd) {
        std::cout << "|Addr: " << curr << " ,Val: " << *reinterpret_cast<void **>(curr) << "|\n";

        curr = VOID_PTR_ADD(curr, 1);
    }
}

#endif