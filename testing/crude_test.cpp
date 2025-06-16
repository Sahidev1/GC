#include "../src/gc.h"
#include <iostream>

using namespace std;

template <typename K> void println(K arg) { cout << arg << "\n"; }

struct goof{
    int buff[1000];
    char* descriptor;
};

void init_goof(goof &g, char* descriptor){
    g.descriptor = descriptor;
}

#define ELEM_COUNT 1000
#define SAFE_PTR_ADD(num) (num % ELEM_COUNT)

int main() {

    Gc gc;

    goof *ptr;

    const unsigned int ELEM_CNT = 1000;
    gc.gc_allocate(&ptr, ELEM_CNT);

    string s = "cool";

    init_goof(*(ptr + SAFE_PTR_ADD(1232)), s.data());

    
    println( (ptr + SAFE_PTR_ADD(1232))->descriptor);


    println(gc.gc_run());

    return 0;
}