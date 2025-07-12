
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>

enum Alloc_errors {
    DEALLOCING_NULL_POINTER = 1,
};


/**
 * Type T is Type you want to allocate data to.
 * Allocation will then be in blocks of size of the type T.
 */
template<typename T>
class Allocator {

  public:
    T* allocate(size_t elems) {
        T* alloc_ptr;
        alloc_ptr = reinterpret_cast<T*>(malloc(sizeof(T) * elems));
        return alloc_ptr;
    }



    T* callocate(size_t num_elems) {
        T* alloc_ptr; 
        alloc_ptr = (T*) calloc(num_elems, sizeof(T));
        return alloc_ptr;
    }

    T* zero_seg_alloc(size_t elems){
        return callocate(elems);
    }

    T* reallocate(T *alloc_ptr, size_t new_nr_elems) {
        alloc_ptr = (T*) realloc(alloc_ptr, sizeof(T) * new_nr_elems);
        return alloc_ptr;
    }

    int deallocate(T *alloc_ptr) {
        if (alloc_ptr == NULL)
            return DEALLOCING_NULL_POINTER;
        free(alloc_ptr);
        return 0;
    }
};



#endif