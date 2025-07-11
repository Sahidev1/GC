
#include "../src/gc.h"
#include <iostream>
#include <cstring>

using namespace std;

struct test_struct {
    long int value;
    char* name;
};


void assign(Gc* gc, test_struct &test, const char* name,long int val){
    test.value = val;
    
    

    GC_FULL_ALLOCATE(gc, char, ptr, strlen(name));

    for(int i = 0; i < strlen(name); i++){
        ptr[i] = name[i];
    }

    test.name = ptr;
    
}

void iterate(Gc &gc) {
    vector<heap_chunk *> alloc_vect = gc.getAllocVector();

    auto it = alloc_vect.begin();
    auto end = alloc_vect.end();

    heap_chunk *chunk;
    int i = 0;
    while (it != end) {
        chunk = *it;
        cout << "Chunk " << i++ << ": flag: " << chunk->flags << ", size: " << chunk->size << "\n";
        it++;
    }
}


int main(){
    #ifndef DEBUG
        cout << "Please compile with debug flag!\n";
        return 1;
    #endif

    #ifdef DEBUG
    Gc gc;
    

    const int ELEM_CNT = 10;
    test_struct* s0;

    //gc.gc_allocate(&s0, ELEM_CNT);
    GC_ALLOCATE(&gc, s0, ELEM_CNT);

    assign(&gc, s0[3 % ELEM_CNT], "cool", 23);
    assign(&gc, s0[7 % ELEM_CNT], "technocolor doop", 2282);

    cout << "alloc count: " << gc.alloc_count << "\n";

    int* arr; 
    
    for(int i = 0; i < 1000; i++){
        gc.gc_allocate(&arr, 1000);
        arr[i] = 2*i;
    }

    int loc_arr[] = {2,3,4,1};
    arr = loc_arr;

    

    iterate(gc);

    gc.run();

    cout << "POST GC_RUN\n";

    iterate(gc);

    GC_MAN_FREE(&gc, s0);

    cout << "POST MANUAL FREE\n";
    iterate(gc);

    #endif




    return 0;
}
