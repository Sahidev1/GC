#ifndef STACKSCANNER_H
#define STACKSCANNER_H

// #ifndef DEBUG
// #define DEBUG
// #endif

#ifdef DEBUG
// put debugging code here

#define DEBUG_ASSERT(arg) assert(arg)
#define DEBUG_RETCODE(code) (DEBUG_ASSERT(code == 0))

#endif

#define POSIX_CHECK()                                                                                                  \
    (defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__linux__) ||              \
     defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__sun))

#define WINDOWS_CHECK() (defined(_WIN32) || defined(_WIN64))

#if POSIX_CHECK()

#include <pthread.h>
#define USING_PTHREADS

#elif WINDOWS_CHECK()
#error "No support for windows thread API's, only POSIX support"
#else
#error "Operating system does not support POSIX threads"
#endif

#include <cstdint>
#include <memory>

#define VOID_PTR_ADD(ptr, val) ((void *)(static_cast<char *>(ptr) + ((val) * sizeof(void *))))

namespace MemoryScanner {
/**
 * size_t index: internal index keept by scanner
 * bool at_end: flag wheter scanner reached end of stack
 * curr: current 8 byte stack segment data, DO NOT CAST TO VOID* AND ACCESS IT UNLESS 100% IT IS A VALID POINTER!
 */
struct StackIterator {
    void *stack_ref;
    size_t index;
    bool at_end;
    uintptr_t curr;
};

class StackScanner {
  private:
    void *stack_addr;
    size_t stack_size;
    pthread_attr_t *attr;
    void init(pthread_t thread_id);

  public:
    StackScanner(/* args */);
    StackScanner(pthread_t thread_id);
    ~StackScanner();
    std::unique_ptr<StackIterator> createIterator(void *stack_ref);
    void scanNext(StackIterator &scanner);
    size_t getStackSize();

#ifdef DEBUG
    void *getStackAddr();
#endif
};

}; // namespace MemoryScanner
#endif