#include "../src/stackscanner.h"
#include "TestUtil.h"
#include <cassert>
#include <iostream>
#include <memory>

using namespace MemoryScanner;

void Assert_non_null_scanner(StackScanner *s) { TEST_ASSERTION(s != nullptr); }

void Assert_stack_scanner_stack_addr_not_null(StackScanner *s) { TEST_ASSERTION(s->getStackAddr() != nullptr); }

void Assert_stack_scanner_stack_addr_reasonable(StackScanner *s) {
    int stack_var;
    void *stack_addr = s->getStackAddr(); // returns base of it
    size_t stack_size = s->getStackSize();
    void *stack_var_addr = &stack_var;
    void *stack_end = VOID_PTR_ADD(stack_addr, stack_size / sizeof(void *));
    TEST_ASSERTION(stack_var_addr >= stack_addr && stack_var_addr < stack_end);
}

void Assert_stack_scanner_created_scanner_not_null(StackScanner *s) {
    std::unique_ptr<StackIterator> scanner = std::make_unique<StackIterator>(*s->createIterator());

    TEST_ASSERTION(s != nullptr);
}

void Assertions_for_stack_scanner_scanner(StackScanner *s) {
    std::unique_ptr<StackIterator> scanner = std::make_unique<StackIterator>(*s->createIterator());



    int arr_size = 1024;
    unsigned long int rand_test = 0x2fab113dff7bcb33;
    unsigned long int arr[arr_size];
    for (size_t i = 0; i < arr_size; i++) {
        arr[i] = rand_test;
    }

    s->scanNext(*scanner);
    TEST_ASSERTION(scanner->curr != 0);
    TEST_ASSERTION(scanner->at_end == false);

    bool match = false;
    while(!scanner->at_end){
        if(!match && ((unsigned long int) scanner->curr) == rand_test){
            match = true;
        }

        
        //TEST_ASSERTION(scanner->curr != nullptr);
        

        s->scanNext(*scanner);
    }

    TEST_ASSERTION(match == true);
    TEST_ASSERTION(scanner->at_end == true);
}

int main() {
#ifndef DEBUG
#error "Please compile in debug mode by providing preprocessor with DEBUG definition: -D DEBUG"
#endif

    SET_PROG_FAIL_CODE(1);
    SET_VERBOSE_SUCCESS();
    DISABLE_FATAL_FAIL();

    StackScanner *stackScanner = new StackScanner();

    Assert_non_null_scanner(stackScanner);
    Assert_stack_scanner_stack_addr_not_null(stackScanner);
    Assert_stack_scanner_stack_addr_reasonable(stackScanner);

    Assert_stack_scanner_created_scanner_not_null(stackScanner);

    Assertions_for_stack_scanner_scanner(stackScanner);

    TEST_SUMMARIZE();


    return UNIT_TEST_RETURN_CODE();
}
