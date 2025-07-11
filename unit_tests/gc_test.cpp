#include "../src/gc.h"
#include "TestUtil.h"
#include "../debug_tools/debugger.h"

using namespace GarbageCollector;

struct test_data {
    unsigned int code;
    char name[10];
};

struct node {
    node* next;
    int data;
};

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

void assert_can_handle_cyclical_refs(){
    Gc *gc = new Gc();

    int k = 0;

    node* curr; gc->allocate<node>(&curr, 1);
    node* root = curr;

    curr->data = ++k; 
    gc->allocate<node>(&curr->next, 1);
    curr = curr->next;
    curr->data = ++k;

    gc->run();
    TEST_NAMED_ASSERTION(gc->alloc_count == 2, "check alloc count");

    gc->allocate<node>(&curr->next, 1);
    node* prev = curr;
    curr = curr->next;
    curr->data = ++k;
    curr->next = prev; prev = nullptr; 
    root = nullptr;
    TEST_NAMED_ASSERTION(gc->alloc_count == 3, "check alloc count");
    


    gc->run();
    DEBUGGER_PRNTLN(gc->alloc_count);
    TEST_NAMED_ASSERTION(gc->alloc_count == 2, "check alloc count after root is nulled");

    curr = nullptr;
    gc->run();
    TEST_ASSERTION(gc->alloc_count == 0);



    delete gc;
}


int main_test(){

    Gc* gc = new Gc();

    TEST_NAMED_ASSERTION(gc->run() == 0, "assert dry run works");

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
    delete gc;
    

    return 0;
}





int main(){
    
    DISABLE_FATAL_FAIL();
    SET_VERBOSE_SUCCESS();
    SET_PROG_FAIL_CODE(1);

    assert_can_handle_cyclical_refs();

    main_test();
}



