//
// Created by os on 8/17/22.
//

#ifndef PROJECT_BASE_V1_1_SEMAPHOREMANAGER_HPP
#define PROJECT_BASE_V1_1_SEMAPHOREMANAGER_HPP

#include "../lib/hw.h"
#include "../h/TCB.hpp"
#include "../h/syscall_c.h"

class SemaphoreManager{

public:

    void *operator new(size_t size) { return MemoryAllocator::before(size); }

    //void operator delete(void *ptr) { MemoryAllocator::deallocate((char*)ptr);}

    void *operator new[](size_t n) { return MemoryAllocator::before(n);}

    //void operator delete[](void *p) noexcept { MemoryAllocator::deallocate((char*)p);}

    SemaphoreManager(sem_t* handle,int init):val(init){
        *handle=this;
    }
    SemaphoreManager()=default;

    ~SemaphoreManager(){ close(); }

    static SemaphoreManager* open(sem_t *handle,int init);

    int close();

    int wait();

    int signal();


private:
    int val;

    List<TCB> blocked;


protected:
    int block();

    int unblock();






};

#endif //PROJECT_BASE_V1_1_SEMAPHOREMANAGER_HPP
