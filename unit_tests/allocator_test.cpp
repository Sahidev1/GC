#include "TestUtil.h"
#include "../src/allocator.h"


#define ALLOC_SIZE_BYTES 1<<16

typedef unsigned char byte;

void assert_non_null(Allocator<byte>* allocator){
    TEST_ASSERTION(allocator != nullptr);
}



int main(){


    SET_VERBOSE_SUCCESS();
    DISABLE_FATAL_FAIL();
    SET_PROG_FAIL_CODE(1);

    Allocator<byte>* alloc = new Allocator<byte>();



}