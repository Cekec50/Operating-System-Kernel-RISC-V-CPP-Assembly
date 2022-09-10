//
// Created by os on 5/29/22.
//

#ifndef PROJECT_BASE_V1_0_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_0_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"


struct BlockHeader {
    BlockHeader* next;
    size_t size;
};

class MemoryAllocator {

public:

    static uint64 createdMainThread;

    static BlockHeader* headptr;

    static void initializeHead();

    static void* before(size_t size);

    static void* allocate (size_t size);

    static int tryToJoin (BlockHeader* cur);

    static int deallocate(char* addr);

    static void print();
};



#endif //PROJECT_BASE_V1_0_MEMORYALLOCATOR_HPP
