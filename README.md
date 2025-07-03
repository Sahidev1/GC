# Mark and Sweep Garbage Collector

A mark-and-sweep garbage collector implementation in C++.

*Note: This README was reformatted with assistance from Claude 4.*

## Progress

### Completed
- ✅ **Custom allocator** - Implementation complete and tested
- ✅ **Stack scanner** - Implementation complete and tested  
- ⚠️ **Garbage collector** - Implemented but contains bugs

## TODO (Priority Order)

### High Priority
1. **Fix cyclical reference bug** - The mark phase is not correctly marking cyclical references. Investigation needed in `mark_phase()` function.

### Medium Priority  
2. **Comprehensive testing** - Expand test coverage beyond basic functionality

## Development Status
Currently debugging the core GC algorithm. The foundation components (allocator and stack scanner) are stable and ready for use.
