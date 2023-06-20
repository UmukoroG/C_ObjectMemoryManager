#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H
#include "ObjectManager.h"
#include <stdbool.h>

typedef struct MEMBLOCK MemBlock;

// information needed to track our objects in memory
struct MEMBLOCK
{
  int numBytes;    // how big is this object?
  int startAddr;   // where the object starts
  Ref ref;         // the reference used to identify the object - id
  int count;       // the number of references to this object
  MemBlock *next;  // pointer to next block.  Blocks stored in a linked list.
};


void insertBlock(MemBlock *block);

void removeBlock(Ref ref);

MemBlock *findBlock(Ref ref);

MemBlock *getNextBlock();

void freeBlocks();

bool ListEmpty();

#endif