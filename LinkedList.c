#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include <stdbool.h>
#include <assert.h>

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