
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>

enum Alloc_errors {
    DEALLOCING_NULL_POINTER = 1,
    ALLOC_PTR_REF_IS_NULL
};

#define PTR_NULLCHECK(ptr) (ptr == nullptr)
#define ALLOC_REF_NULLCHECK(ref) if ((ref) == nullptr) return ALLOC_PTR_REF_IS_NULL;



/**
 * Type T is Type you want to allocate data to.
 * Allocation will then be in blocks of size of the type T.
 */
template<typename T>
class Allocator {

  public:
    int allocate(T** alloc_ptr, size_t elems) {
        ALLOC_REF_NULLCHECK(alloc_ptr);

        *alloc_ptr = malloc(sizeof(T) * elems);
        return PTR_NULLCHECK(*alloc_ptr);
    }



    int callocate(T **alloc_ptr, size_t num_elems) {
        ALLOC_REF_NULLCHECK(alloc_ptr);
        
        *alloc_ptr = (T*) calloc(num_elems, sizeof(T));
        return PTR_NULLCHECK(*alloc_ptr);
    }

    int zero_alloc(T **alloc_ptr, size_t elems){
        return callocate(alloc_ptr, elems);
    }

    int reallocate(T **alloc_ptr, size_t new_nr_elems) {
        ALLOC_REF_NULLCHECK(alloc_ptr);

        *alloc_ptr = realloc(*alloc_ptr, sizeof(T) * new_nr_elems);
        return PTR_NULLCHECK(*alloc_ptr);
    }

    int deallocate(T **alloc_ptr) {
        ALLOC_REF_NULLCHECK(alloc_ptr);

        if (*alloc_ptr == NULL)
            return DEALLOCING_NULL_POINTER;
        free(*alloc_ptr);
        return 0;
    }
};



#endif