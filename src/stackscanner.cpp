#include "stackscanner.h"
#include "../debug_tools/debugger.h"

#include <iostream>

using namespace std;
using namespace MemoryScanner;

/** create stack scanner thats scan current threads stackspace */
StackScanner::StackScanner() {
    pthread_t s = pthread_self();
    this->init(s);
    
}

/** create stack scanner for a thread */
StackScanner::StackScanner(pthread_t thread_id){
    this->init(thread_id);
}

/** Creates a new initialized StackIterator object */
std::unique_ptr<StackIterator> StackScanner::createIterator(void* stack_ref){
    unique_ptr<StackIterator> scanner = make_unique<StackIterator>();
    scanner->stack_ref = stack_ref;
    scanner->curr = 0;
    scanner->index = 0;
    scanner->at_end = false;
    return scanner;
}

/** Moves scanner to next non zero 8 byte stack data segment */
void StackScanner::scanNext(StackIterator &scanner){
    void* top = (void*)(((void**) this->stack_addr) + (this->stack_size/sizeof(void*)));
    //void* start = VOID_PTR_ADD(this->stack_addr, scanner.index);
    void* start = VOID_PTR_ADD(scanner.stack_ref, scanner.index);


    int hits = 0;

    
    

    void* tmp;
    while(start < top){
        if ((tmp = *((void**) start)) != nullptr) {
            scanner.curr = reinterpret_cast<intptr_t>(tmp);
            scanner.index++;
            hits++;
            return;
        }
        scanner.index++;
        start = VOID_PTR_ADD(start, 1);
    }
    scanner.at_end = true;
    scanner.curr = 0;
}

/** size of stack in bytes */
size_t StackScanner::getStackSize() { return this->stack_size; }

#ifdef DEBUG
    void *StackScanner::getStackAddr() { return this->stack_addr; }
#endif

/**
 * initialization based on target thread. 
 * gets thread thread attributes such as address and size. 
 */
void StackScanner::init(pthread_t thread_id) {
    int rc;
    pthread_attr_t *attr = new pthread_attr_t();
    //rc = pthread_attr_init(attr);
   

    rc = pthread_getattr_np(thread_id, attr);
    DEBUG_RETCODE(rc);

    //DEBUG_CHECK();
    void *stackaddr;
    size_t stacksize;
    //DEBUG_CHECK();
    rc = pthread_attr_getstack(attr, &stackaddr, &stacksize);
    //DEBUG_CHECK();
    DEBUG_RETCODE(rc);
    

    this->stack_addr = stackaddr;
    this->stack_size = stacksize;
    this->attr = attr;

    DEBUGGER_PRNTLN("\nstack base: " << stack_addr << ", top: " << VOID_PTR_ADD(stack_addr, (stack_size/sizeof(void*))) << ", size: " << stack_size <<"\n"); 

}



StackScanner::~StackScanner() {
    pthread_attr_destroy(this->attr);
}


