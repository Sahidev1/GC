#include "../src/gc.h"
#include "TestUtil.h"

using namespace GarbageCollector;

void assert_non_null_gc_instance(Gc* gc){
    TEST_ASSERTION(gc != nullptr);
}

struct test_data {
    unsigned int code;
    char name[10];
};



int main(){
    
    SET_VERBOSE_SUCCESS();
    SET_PROG_FAIL_CODE(1);

    Gc* gc = new Gc();
    assert_non_null_gc_instance(gc);
    const unsigned long ALLOC_ELEMS = (2000);

    const unsigned long CODE = 2421421;

    test_data* data = gc->allocate<test_data>(ALLOC_ELEMS);
    TEST_NAMED_ASSERTION(data != nullptr, "assert allocation non null");
    TEST_NAMED_ASSERTION(gc->alloc_count == 1, "assert allocation count non zero");

    //std::vector<heap_chunk*> vector = gc->getAllocVector();
    
    data[33].code = CODE;

    TEST_NAMED_ASSERTION(data[33].code == CODE, "assert data access"); 

    

    TEST_NAMED_ASSERTION(gc->run() == 0, "gc run");
    TEST_NAMED_ASSERTION(gc->alloc_count == 1, "assert allocation count non zero");


    TEST_NAMED_ASSERTION(data[33].code == CODE, "non garbage collected data is not corrupted"); 

    data = nullptr;


    TEST_NAMED_ASSERTION(gc->run() == 0,"GC run success!");

    TEST_NAMED_ASSERTION(gc->alloc_count == 0, "assert allocation has been GCed");

    return 0;
}



