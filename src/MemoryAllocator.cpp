//
// Created by os on 5/29/22.
//
#include "../../project-base-v1.1/h/MemoryAllocator.hpp"
#include "../../project-base-v1.1/h/Print.hpp"

BlockHeader* MemoryAllocator::headptr=nullptr;
uint64 MemoryAllocator::createdMainThread = 0;
void MemoryAllocator::initializeHead(){
    if(MemoryAllocator::headptr==nullptr){
        MemoryAllocator:: headptr=(BlockHeader*)HEAP_START_ADDR;
        MemoryAllocator::headptr->size=(size_t)((char*)HEAP_END_ADDR-(char*)HEAP_START_ADDR);
        MemoryAllocator::headptr->next=nullptr;
    }
}

//
void* MemoryAllocator::before(size_t size){
    if (size==0)
        return nullptr;
    size_t newsize=size+sizeof(BlockHeader);
    if (newsize<64)
        newsize = 64;
    else if(newsize>1024) {
        return nullptr;
    }
    size_t abiSize=0;

    if((newsize) % MEM_BLOCK_SIZE)
        abiSize=(newsize)/MEM_BLOCK_SIZE + 1;
    else
        abiSize=(newsize)/MEM_BLOCK_SIZE;

    return MemoryAllocator::allocate(abiSize*MEM_BLOCK_SIZE);

}


void* MemoryAllocator::allocate (size_t size) {
// Try to find an existing free block in the list (first fit):
    BlockHeader *blk = (BlockHeader*)MemoryAllocator::headptr, *prev = nullptr;
    for (; blk!=nullptr; prev=blk, blk=blk->next)
        if (blk->size>=size) break;
// If not found, allocate a new memory segment and add it to the list:
    if (blk == nullptr) {
        return nullptr;
    }
// Allocate the requested block:
    size_t remainingSize = blk->size - size;
    if (remainingSize >= sizeof(BlockHeader) + MEM_BLOCK_SIZE) {
// A fragment remains
        blk->size = size;
        //size_t offset = sizeof(BlockHeader) + size;
        BlockHeader* newBlk = (BlockHeader*)((char*)blk + size);
        if (prev) prev->next = newBlk;
        else MemoryAllocator::headptr = newBlk;
        newBlk->next = blk->next;
        newBlk->size = remainingSize;
    } else {
// No remaining fragment, allocate the entire block
        if (prev) prev->next = blk->next;
        else MemoryAllocator::headptr = blk->next;
    }
    blk->next = nullptr;
    return (char*)blk + sizeof(BlockHeader);
}


// Helper: Try to join cur with the cur->next free segment:
int MemoryAllocator::tryToJoin (BlockHeader* cur){

    if (!cur) return 0;
    if (cur->next && (char*)cur+cur->size == (char*)(cur->next)) {
// Remove the cur->next segment:
        cur->size += cur->next->size;
        cur->next = cur->next->next;
        //if (cur->next) cur->next->prev = cur;
        return 1;
    } else
        return 0;
}

int MemoryAllocator::deallocate(char* addr){
    if(!addr)
        return -1;
    BlockHeader* sizeHeader = (BlockHeader*)(addr - sizeof(BlockHeader));
    size_t size = sizeHeader->size;

    // Find the place where to insert the new free segment (just after cur):
    BlockHeader* cur=nullptr;
    if (!MemoryAllocator::headptr || addr<(char*)MemoryAllocator::headptr)
        cur = nullptr; // insert as the first
    else
        for (cur=MemoryAllocator::headptr; cur->next!=0 && addr>(char*)(cur->next);
             cur=cur->next);
// Insert the new segment after cur:
    BlockHeader* newSeg = sizeHeader;
    newSeg->size = size;
    //newSeg->prev = cur;
    if (cur) newSeg->next = cur->next;
    else newSeg->next = MemoryAllocator::headptr;
    //if (newSeg->next) newSeg->next->prev = newSeg;
    if (cur) cur->next = newSeg;
    else MemoryAllocator::headptr = newSeg;
// Try to merge with the previous and next segments:
    tryToJoin(newSeg);
    tryToJoin(cur);
    return 0;
}

void MemoryAllocator::print(){
    BlockHeader* curr=MemoryAllocator::headptr;
    while(curr){
        printStringUser("->");
        printStringUser("[");
        printIntegerUser(size_t(curr));
        printStringUser(",");
        printIntegerUser(curr->size);
        printStringUser("]");
        curr=curr->next;
    }
    printStringUser("\n");
}

