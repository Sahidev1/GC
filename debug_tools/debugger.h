#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>


#define DEBUGGER_MESSAGE(line) "\tDEBUG PRINT @ "<<__FILE__<<":"<<__LINE__<<" --->"<< line
#define DEBUGGER_PRNTLN(line) do\
{   \
    std::cout << DEBUGGER_MESSAGE(line) << "\n";\
} while (0);\




#define DEBUGGER_COND_PRNTLN(cond, line) ((cond)?DEBUGGER_PRNTLN(line):void)

#endif