#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>



static bool debug_enabled = true;



#define DEBUGGER_DISABLE() (debug_enabled = false)
#define DEBUGGER_ENABLE() (debug_enabled = true)

#define DEBUGGER_MESSAGE(line) "\tDEBUG PRINT @ "<<__FILE__<<":"<<__LINE__<<" --->"<< line
#define DEBUGGER_PRNTLN(line) do\
{   \
    if(!debug_enabled) break; \
    std::cout << DEBUGGER_MESSAGE(line) << "\n";\
} while (0);\




#define DEBUGGER_COND_PRNTLN(cond, line) ((cond)?DEBUGGER_PRNTLN(line):void)

#endif