
Developing a mark and sweep garbage collector in C++. 

Progress: 

*costum allocator implementation, done, tested.
*stack scanner implementation, done, tested.
*gc implemented, but has bugs. 

gc todos in order of priority: 
*fix bug with cyclical references, it is not marking it correctly for whatever reason. check mark_phase function. 
*more comperehensive testing.  