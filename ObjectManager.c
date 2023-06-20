#include <stdio.h>
#include <stdlib.h>
#include "ObjectManager.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

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

//List and helper functions
void insertBlock(MemBlock *block);

void removeBlock(Ref ref);

MemBlock *findBlock(Ref ref);

MemBlock *getNextBlock();

void freeBlocks();

bool ListEmpty();

void compact();

MemBlock *allocateMemory(unsigned long size);
//

// tracks the next reference (ID) to use, we start at 1 so we can use 0 as the NULL reference
static Ref nextRef = 1;

// The blocks are stored in a linked list where the start of the list is pointed to by memBlockStart.
static MemBlock *memBlockStart; // start of linked list of blocks allocated
static MemBlock *memBlockEnd;   // end of linked list of blocks allocated
static int numBlocks;            // number of blocks allocated

// our buffers.  This is where we allocate memory from.  One of these is always the current buffer.  The other is used for swapping
//during compaction stage.
static unsigned char buffer1[MEMORY_SIZE];
static unsigned char buffer2[MEMORY_SIZE];

// points to the current buffer containing our data
static unsigned char *currBuffer = buffer1;

// points to the location of the next available memory location
static int freeIndex = 0;

// performs required setup
void initPool()
{
  nextRef=1;
  freeIndex=0;  
}

// performs required clean up
void destroyPool()
{
  //we want to delete all nodes from the linked list.
  if(currBuffer==NULL){
    fprintf(stderr, "Object Manager started\n");
  };

  freeBlocks();
  
}

// Adds the given object into our buffer. It will fire the garbage collector as required.
// We always assume that an insert always creates a new object...
// On success it returns the reference number for the block allocated.
// On failure it returns NULL_REF (0)
Ref insertObject( const int size )
{
  if(currBuffer==NULL){
    fprintf(stderr, "Object Manager not started\n");
    return NULL_REF;
  };
  
  if(size<=0){
    fprintf(stderr, "The ref value is not valid\n");
    return NULL_REF;
  }
  if(freeIndex + size > MEMORY_SIZE){
    compact();
    if(freeIndex + size > MEMORY_SIZE){
      fprintf(stderr, "Ran out of memory\n");
      return NULL_REF;
    }
  }
  MemBlock *newBlock=allocateMemory(size);
  if(newBlock==NULL){
    return NULL_REF;
  }

  insertBlock(newBlock);
  return newBlock->ref;  
}

void *retrieveObject( const Ref ref )
{
  if(currBuffer==NULL){
    fprintf(stderr, "Object Manager not started\n");
    return NULL;
  };
  
  if(ref==NULL_REF){
    fprintf(stderr, "The ref value is not valid\n");
    return NULL;
  }
  
  MemBlock *block=findBlock(ref); 
  if(block==NULL){
    fprintf(stderr, "Object with the given ref not found\n");
    return NULL;
  }

  int start=block->startAddr;
  if(start>=0 && start<MEMORY_SIZE){
    unsigned char *tmpBuffer=(unsigned char *)currBuffer;
    return &(tmpBuffer[start]);
  }
  return NULL;
}

void addReference( const Ref ref )
{
  if(currBuffer==NULL){
    fprintf(stderr, "Object Manager not started\n");
    return;
  };

  MemBlock *block=findBlock(ref); 
  if(block==NULL){
    return;
  }
  block->count++;  
}

void dropReference( const Ref ref )
{
  if(currBuffer==NULL){
    fprintf(stderr, "Object Manager not started\n");
    return;
  };

  MemBlock *block=findBlock(ref); 
  if(block==NULL){
    return;
  }
  block->count--;

  if(block->count<=0){
    removeBlock(ref);
  } 
}



void compact(){
  if (currBuffer == NULL) {
    fprintf(stderr, "Object Manager not started\n");
    return;
  }

  unsigned char* secondBuffer = buffer2;
  unsigned char* secondTmpBuffer = secondBuffer;
  unsigned char* tmpBuffer = currBuffer;
  unsigned long secondFreePtr = 0;

  MemBlock* traversal=getNextBlock();
  int numOfExistingObjects = 0;
  unsigned long currentNumberOfBytesInUse = 0;
  unsigned long numberOfBytesCollected = 0;
  int i;

  if (ListEmpty() == true) {
    freeIndex = 0;
    return;
  }

  while (traversal != NULL) {
    unsigned long size = traversal->numBytes;
    unsigned long start = traversal->startAddr;

    for (i = 0; i < size; i++) {
      secondTmpBuffer[secondFreePtr + i] = tmpBuffer[start + i];
    }

    traversal->startAddr = secondFreePtr;
    secondFreePtr += size;
    numOfExistingObjects++;
    currentNumberOfBytesInUse += size;

    traversal = getNextBlock();
  }

  numberOfBytesCollected = freeIndex - secondFreePtr;

  memcpy(buffer2, currBuffer, MEMORY_SIZE);
  memcpy(currBuffer, buffer1, MEMORY_SIZE);
  memcpy(buffer1, buffer2, MEMORY_SIZE);

  freeIndex = secondFreePtr;

  fprintf(stdout, "\nGarbage Collector statistics\n----------------------------------\nNumber of Objects that Exist: %d\nCurrent Number of Bytes in use: %lu\nNumber of Bytes collected: %lu\n", numOfExistingObjects, currentNumberOfBytesInUse, numberOfBytesCollected);
}


void dumpPool()
{
  if(currBuffer == NULL){
    fprintf(stderr, "Object Manager not started\n");
    return;
  };

  MemBlock *traversal=getNextBlock();

  fprintf(stdout,"-----Object List Dump------\n");
  if(traversal==NULL){
    fprintf(stdout,"There is no object\n");
    return;
  }
  while(traversal!=NULL){
    unsigned char *tmpBuffer=(unsigned char *) currBuffer;
    fprintf(stdout,"Ref:%lu\nSize: %lu\nStart address: %d\ncount:%lu\n--------\n", traversal->ref, traversal->numBytes, traversal->startAddr,traversal->count);
    traversal=getNextBlock();
  }
  return;  
}

//you may add additional function if needed

static MemBlock *head = NULL;
MemBlock *traversal = NULL;

void insertBlock(MemBlock *block){
    assert(block!=NULL);
    MemBlock *traversal =head;
    if(head==NULL){
        head=block;
        return;
    }
    while(traversal->next){
        traversal = traversal->next;
    }
    traversal->next=block;
    return;
}

void removeBlock(Ref ref){
    MemBlock *traversal=head;
    if(head==NULL){
        return;
    }

    if(ref==head->ref){
        MemBlock *tmp=head;
        head=head->next;
        free(tmp);
        return;
    }
    while(traversal->next){
        MemBlock *nextBlock=traversal->next;
        if(ref=nextBlock->ref){
            traversal->next=nextBlock->next;
            free(nextBlock);
            return;
        }
        traversal=traversal->next;
    }
    return;
}

MemBlock *findBlock(Ref ref){
    MemBlock *traversal=head;
    while(traversal){
        if(ref==traversal->ref){
            return traversal;
        }
        traversal=traversal->next;
    }
    return NULL;
}

MemBlock *getNextBlock(){
    if(head==NULL){
        return NULL;
    }
    if(traversal==NULL){
        traversal=head;
        return traversal;
    }
    if(traversal->next==NULL){
        traversal=NULL;
        return NULL;
    }
    traversal=traversal->next;
    if(traversal==NULL){
        return NULL;
    }
    return traversal;
}   

void freeBlocks(){
    MemBlock *traversal=head;
    while (traversal){
        MemBlock *tmp=traversal;
        traversal=traversal->next;
        free(tmp);
    }
    head=NULL;
} 

bool ListEmpty(){
    if(head==NULL){
        return true;
    }
    return false;
}
MemBlock *allocateMemory(unsigned long size){
  if(size<=0 || freeIndex + size > MEMORY_SIZE){
    return NULL;
  }

  MemBlock *createdBlock = (MemBlock *) malloc(sizeof(MemBlock));
  createdBlock->numBytes=size;
  createdBlock->startAddr=freeIndex;
  createdBlock->count=1;
  createdBlock->next=NULL;
  createdBlock->ref = nextRef++;

  freeIndex+=size;
  return createdBlock;
}
