#include "../src/gc.h"
#include "TestUtil.h"
#include "../debug_tools/debugger.h"

using namespace GarbageCollector;

void assert_non_null_gc_instance(Gc* gc){
    TEST_ASSERTION(gc != nullptr);
}

void assert_immediate_alloc_dealloc_gc_run_consistent(Gc &gc){
    int init_count = gc.alloc_count;
    char* p;
    gc.allocate<char>(&p, 100);
    p = nullptr;
    gc.run();
    TEST_ASSERTION(gc.alloc_count == init_count);

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
    const unsigned long ALLOC_ELEMS = (1<<16);

    const unsigned long CODE = 2421421;

    test_data* data;
    gc->allocate<test_data>(&data, ALLOC_ELEMS);
    TEST_NAMED_ASSERTION(data != nullptr, "assert allocation non null");
    TEST_NAMED_ASSERTION(gc->alloc_count == 1, "assert allocation count non zero");

    //std::vector<heap_chunk*> vector = gc->getAllocVector();
    
    data[33].code = CODE;

    TEST_NAMED_ASSERTION(data[33].code == CODE, "assert data access"); 
    

    TEST_NAMED_ASSERTION(gc->run() == 0, "gc run");
    TEST_NAMED_ASSERTION(gc->alloc_count == 1, "assert allocation count non zero");
    TEST_NAMED_ASSERTION(data[33].code == CODE, "non garbage collected data is not corrupted"); 

    assert_immediate_alloc_dealloc_gc_run_consistent(*gc);

    data = nullptr;

    int* p0;
    gc->allocate<int>(&p0,10);
    p0 = nullptr;
    TEST_NAMED_ASSERTION(gc->run() == 0,"GC run success!");
    TEST_NAMED_ASSERTION(gc->alloc_count == 0, "assert allocation has been GCed");
    


    TEST_SUMMARIZE();
    

    return 0;
}



