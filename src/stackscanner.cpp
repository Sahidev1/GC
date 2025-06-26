#include "stackscanner.h"

#include <iostream>

using namespace std;

/** create stack scanner thats scan current threads stackspace */
Stackscanner::Stackscanner() {
    pthread_t s = pthread_self();
    this->init(s);
    
}

/** create stack scanner for a thread */
Stackscanner::Stackscanner(pthread_t thread_id){
    this->init(thread_id);
}

/** Creates a new initialized Scanner object */
Scanner *Stackscanner::createScanner(){
    Scanner *scanner = new Scanner();
    scanner->curr = 0;
    scanner->index = 0;
    scanner->at_end = false;
    return scanner;
}

/** Moves scanner to next non zero 8 byte stack data segment */
void Stackscanner::scanNext(Scanner &scanner){
    void* start = VOID_PTR_ADD(this->stack_addr, scanner.index);
    void* top = VOID_PTR_ADD(this->stack_addr, (this->stack_size/sizeof(void*)));


    void* tmp;
    while(start < top){
        if ((tmp = *((void**) start)) != nullptr) {
            scanner.curr = reinterpret_cast<intptr_t>(tmp);
            scanner.index++;
            return;
        }
        scanner.index++;
        start = VOID_PTR_ADD(start, 1);
    }
    scanner.at_end = true;
    scanner.curr = 0;
}

/** size of stack in bytes */
size_t Stackscanner::getStackSize() { return this->stack_size; }

#ifdef DEBUG
    void *Stackscanner::getStackAddr() { return this->stack_addr; }
#endif

/**
 * initialization based on target thread. 
 * gets thread thread attributes such as address and size. 
 */
void Stackscanner::init(pthread_t thread_id) {
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

}



Stackscanner::~Stackscanner() {
    pthread_attr_destroy(this->attr);
}


