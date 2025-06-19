#include "../src/gc.h"
#include <iostream>

using namespace std;

template <typename K> void println(K arg) { cout << arg << "\n"; }

struct goof{
    int buff[1000];
    char* descriptor;
};

struct node{
    int a;
    struct node *next;
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


    int* iptr0;

    gc.gc_allocate(&iptr0, 10000);

    char* cptr;
    for(int i = 0; i < 4000; i++){
        gc.gc_allocate(&cptr, 100);
    }

    node* root;
    gc.gc_allocate(&root, 1);
    root->a = 11;


    node* traverse = root;
    node* tmp;

    for(int i = 0; i < 202439; i++){
        gc.gc_allocate(&tmp,1);
        traverse->next = tmp;
        traverse = traverse->next;
    }


    println(gc.gc_run());

    return 0;
}