The purpose of this program is to implement a mark and sweep defragmentation/mark-compact garbage collector, which is a memory management technique used in some garbage collectors to reclaim memory occupied by unreachable objects. It consists of two main phases: marking and sweeping.

Mark Phase: In this phase, the garbage collector traverses the object graph starting from known root objects (e.g., global variables, stack frames) and marks all reachable objects. 

Sweep Phase: Once the mark phase is complete, the sweep phase begins. In this phase, the garbage collector iterates over the entire heap and reclaims memory occupied by unmarked (unreachable) objects. It compacts the memory by moving the live objects closer together, effectively eliminating fragmentation.


To compile the code, cd into program then run make in the terminal

LinkedList.h; LinkedList.c contains the information that manages the associated objects

ObjectManager.h; ObjectManager.c implements the garbage collector

Main0.c; this calls all our function implemented as part of the garbage collector.

