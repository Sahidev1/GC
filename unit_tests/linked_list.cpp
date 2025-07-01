
#include "../src/gc.h"
#include <cassert>
#include <iostream>



using namespace std;
using namespace GarbageCollector;

template<typename T>
struct node {
    T data;
    node<T>* next;
};



int main(){

    #ifdef DEBUG

    Gc gc_inst;
    Gc *gc = &gc_inst;
    assert(gc != nullptr);

    node<int> root;

    node<int>* iterator = &root;

    const unsigned int NODE_CNT = 100;

    auto alloc_v = gc->getAllocVector();

    node<int>* tmp = nullptr;
    for (int i = 0; i < NODE_CNT; i++){
        assert(iterator != nullptr);
        iterator->data = 2*i;
        assert(gc->allocate(&tmp, 1) == 0);
        assert(tmp != nullptr);
        iterator->next = tmp;
        iterator = iterator->next;
        tmp = nullptr;    
    }

    assert(gc->alloc_count == NODE_CNT);

    iterator = &root;
    for(int i = 0; i < NODE_CNT/2; i++){
        iterator = iterator->next;
    }
    assert(iterator != nullptr);
    iterator->next = nullptr;


    assert(gc->run() == 0);
    assert(gc->alloc_count < NODE_CNT);
    cout << gc->alloc_count << "\n";





    #endif



    return 0;
}