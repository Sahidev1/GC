#include "../debug_tools/debugger.h"
#include "TestUtil.h"
#include "../src/gc.h"
#include <vector>

using namespace GarbageCollector;

template<typename K>
struct node {
    K data;
    node* next;
};

void singe_alloc_check(){
    Gc gc;
    TEST_ASSERTION(gc.alloc_count == 0);
    node<int> *n;
    int retc = gc.allocate<node<int>>(&n,1);
    TEST_ASSERTION(retc == 0);
    TEST_ASSERTION(gc.alloc_count == 1);
    n->data = 12;

    auto vect = gc.getAllocVector();
    for(auto it = vect.begin(); it < vect.end();){
        DEBUGGER_PRNTLN(**it);
        it++;
    }

    retc = gc.run();

    TEST_ASSERTION(retc == 0);
    TEST_ASSERTION(gc.alloc_count == 1);

    n = nullptr;
    retc = gc.run();
    TEST_ASSERTION(retc == 0);
}

void cyclical_ref_test_2_refs() {
    Gc gc;

    int retc;
    node<int> *n; //root ref
    retc = gc.allocate<node<int>>(&n, 1);
    
    TEST_NAMED_ASSERTION(retc == 0, "first allocation success");
    n->data = 3;

    retc = gc.allocate<node<int>>(&n->next, 1);
    TEST_NAMED_ASSERTION(retc == 0, "second allocation success");

    n->next->next = n;
    n->next->data = 5;

    TEST_NAMED_ASSERTION(n == n->next->next, "is cyclical");
    TEST_NAMED_ASSERTION(gc.alloc_count == 2, "check alloc count");

    auto vect = gc.getAllocVector();
    for(auto it = vect.begin(); it < vect.end(); it++){
        DEBUGGER_PRNTLN("heap chunk data seg addr: " << *it + 1 << "\n");
        DEBUGGER_PRNTLN(**it);
        
    }

    retc = gc.run();
    TEST_ASSERTION(retc == 0);
    TEST_NAMED_ASSERTION(gc.alloc_count == 2, "check alloc count");
    IF_FAILED(DEBUGGER_PRNTLN("alloc count: " << gc.alloc_count));

    vect = gc.getAllocVector();
    for(auto it = vect.begin(); it < vect.end(); it++){
        DEBUGGER_PRNTLN("heap chunk addr: " << *it + 1 << "\n");
        DEBUGGER_PRNTLN(**it);
        
    }

    using namespace std;
    

    TEST_ASSERTION(vect.size() == 2);
    IF_FAILED(DEBUGGER_PRNTLN(vect.size()));

    n->next = nullptr;
    gc.run();
    TEST_ASSERTION(gc.alloc_count == 1);

    n = nullptr;
    gc.run();

    TEST_ASSERTION(gc.alloc_count == 0);

}

int main(){

    DEBUGGER_DISABLE();
    
    SET_VERBOSE_SUCCESS();
    DISABLE_FATAL_FAIL();
    SET_PROG_FAIL_CODE(1);

    singe_alloc_check();
    cyclical_ref_test_2_refs();
    TEST_SUMMARIZE();


    return UNIT_TEST_RETURN_CODE();
}