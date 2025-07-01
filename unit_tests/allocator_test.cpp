#include "../src/allocator.h"
#include "TestUtil.h"

#define ALLOC_ELEM_CNT 1 << 16

typedef unsigned char byte;

template <typename K> struct Allocation {
    K *data;
    size_t elem_count;
};

void assert_non_null(Allocator<byte> *allocator) { TEST_ASSERTION(allocator != nullptr); }

void test_allocating(Allocator<byte> *allocator) {
    size_t elem_cnt = ALLOC_ELEM_CNT;
    byte *ptr = allocator->allocate(elem_cnt);
    TEST_ASSERTION(ptr != nullptr);
    allocator->deallocate(ptr);
}

void test_callocating(Allocator<byte> *a) {
    size_t elem_cnt = ALLOC_ELEM_CNT;
    byte *ptr = a->callocate(elem_cnt);
    TEST_ASSERTION(ptr != nullptr);
    a->deallocate(ptr);
}

void test_zero_zeg_alloc(Allocator<byte> *a) {
    size_t elem_cnt = ALLOC_ELEM_CNT;
    byte *ptr = a->zero_seg_alloc(elem_cnt);
    TEST_ASSERTION(ptr != nullptr);
    a->deallocate(ptr);
}

void test_reallocate(Allocator<byte> *a) {
    size_t init_cnt = ALLOC_ELEM_CNT / 8;
    byte *ptr = a->allocate(init_cnt);

    bool cond = ptr != nullptr;

    if (cond) {
        size_t new_cnt = init_cnt / 2;
        ptr = a->reallocate(ptr, new_cnt);
        cond = ptr != nullptr;

        if(cond){
            size_t newest_cnt = ALLOC_ELEM_CNT;
            ptr = a->reallocate(ptr, new_cnt);
            cond = ptr != nullptr;
        }
    }

    TEST_ASSERTION(cond);
}

int main() {

    SET_VERBOSE_SUCCESS();
    DISABLE_FATAL_FAIL();
    SET_PROG_FAIL_CODE(1);

    Allocator<byte> *alloc = new Allocator<byte>();
    assert_non_null(alloc);
    test_allocating(alloc);
    test_callocating(alloc);
    test_reallocate(alloc);

    TEST_SUMMARIZE();

    return UNIT_TEST_RETURN_CODE();
}