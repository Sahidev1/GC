#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <cassert>
#include <iostream>

struct config {
    bool fatal_fail;
    bool verbose_fail;
    bool verbose_success;
    int tests;
    int fails;
    int program_failcode;
    bool lastFailed;
};

inline config internal = {true, true, false, 0, 0, 1, false};
inline config* const internal_config = &internal;

#define NOTHING() do {} while (0)

#define PRINTLN(expr) (std::cout << expr << "\n")
#define STR(x) #x
#define EXP_STR(x) STR(x)

#define SET_VERBOSE_FAIL() (internal_config->verbose_fail = true)
#define DISABLE_VERBOSE_FAIL() (internal_config->verbose_fail = false)

#define SET_VERBOSE_SUCCESS() (internal_config->verbose_success = true)
#define DISABLE_VERBOSE_SUCCESS() (internal_config->verbose_success = false)

#define ENABLE_FATAL_FAIL() (internal_config->fatal_fail = true)
#define DISABLE_FATAL_FAIL() (internal_config->fatal_fail = false)

#define SET_PROG_FAIL_CODE(arg) (internal_config->program_failcode = arg)

#define CHECK_VERBOSE_FAIL() (internal_config->verbose_fail)
#define CHECK_VERBOSE_SUCCESS() (internal_config->verbose_success)

#define FAIL_MESSAGE(expr, name) "UNIT_TEST FAIL  on " __DATE__ ", " __TIME__ " in file: " __FILE__ "\n\t-->in function: " << __func__ << ", at line: " << __LINE__ << "\n\t-->on assertion: " << expr << "\n"

#define SUCCESS_MESSAGE(name) "TEST [" name "] PASSED: " << __func__ << "------> line: " << __LINE__


#define ON_FAIL(expr, name) do { \
    internal_config->lastFailed = true;\
    if (CHECK_VERBOSE_FAIL()) PRINTLN(FAIL_MESSAGE(expr,name)); \
} while (0)

#define ON_SUCCESS(name) do { \
    internal_config->lastFailed = false;\
    if (CHECK_VERBOSE_SUCCESS()) PRINTLN(SUCCESS_MESSAGE(name)); \
} while (0)

#define TEST_NAMED_ASSERTION(assertion, name) do { \
    internal_config->tests++;\
    if (assertion) { \
        ON_SUCCESS(name); \
    } else { \
        internal_config->fails++;\
        ON_FAIL(#assertion, name); \
        if(internal_config->fatal_fail) assert(assertion);\
    } \
} while (0)

#define TEST_ASSERTION(assertion) TEST_NAMED_ASSERTION(assertion,)

#define IF_FAILED(expr) do {\
    if(internal_config->lastFailed){\
        PRINTLN("\n\tFAIL CONDITION PROCEDURE EXECUTED!\n");\
        expr\
        PRINTLN("\n\tFAIL CONDITION PROCEDURE FINISHED!\n");\
    } \
} while(0)\



#define TEST_SUMMARIZE() do {\
    PRINTLN("\nUNIT TEST file: " << __FILE__ << ", date: " << __DATE__ << ", time: " << __TIME__ << ", Results: ");\
    PRINTLN("\t"<<internal_config->tests << " assertions, " << internal_config->fails << " failures");\
} while(0)\

#define UNIT_TEST_RETURN_CODE() (internal_config->fails != 0?internal_config->program_failcode:0)


#endif
