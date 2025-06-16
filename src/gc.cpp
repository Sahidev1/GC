#include "gc.h"

void Gc::mark(heap_chunk *chnk) { chnk->flags |= (0x1 << FLAG_MARK_BIT_INDEX); }

int Gc::is_marked(heap_chunk *chnk) { return (chnk->flags >> FLAG_MARK_BIT_INDEX) & 0x1; }

heap_chunk* Gc::data_to_heap_chunk_addr(char *data_addr) {
    heap_chunk *tmp = (heap_chunk *)data_addr;
    return tmp - 1;
}

bool Gc::is_stack_reachable(heap_chunk *chnk) {

    char *stack_ptr = (char *)*(chnk->stack_ptr);
    char *chunk_ptr = (char *)chnk;

    return stack_ptr - sizeof(heap_chunk) == chunk_ptr;
}

std::vector<heap_chunk *> Gc::get_stack_reachable() {
    auto start = alloc_vector.begin();
    auto end = alloc_vector.end();

    std::vector<heap_chunk *> stack_reachable;

    heap_chunk *chnk;
    while (start < end) {
        chnk = *start;
        if (is_stack_reachable(chnk)) {
            stack_reachable.push_back(chnk);
        }
        start++;
    }

    return stack_reachable;
}

int Gc::mark_from_chunk(heap_chunk *chunk, std::vector<heap_chunk *> &reach_able) {
    unsigned int seg_size = sizeof(void *);
    uint64_t seg_count = chunk->size / seg_size;
    char *seg = (char *)(chunk + 1);
    char *last_seg_sentinel = seg + seg_count * seg_size;

    heap_chunk *pot_heap_chunk;
    while (seg < last_seg_sentinel) {
        pot_heap_chunk = data_to_heap_chunk_addr(seg);
        auto it = alloc_set.find(pot_heap_chunk);
        if (it != alloc_set.end() && !is_marked(*it)) {
            mark(*it);
            reach_able.push_back(*it);
        }

        seg += seg_size;
    }

    return 0;
}

int Gc::mark_phase() {
    auto reachable = get_stack_reachable();

    heap_chunk *chunk;
    size_t size;
    while ((size = reachable.size()) > 0) {
        chunk = reachable[size - 1];
        reachable.pop_back();

        if (!is_marked(chunk))
            mark_from_chunk(chunk, reachable);
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

Gc::Gc() : alloc_count(0) {}

int Gc::gc_run() { return mark_phase(); }
