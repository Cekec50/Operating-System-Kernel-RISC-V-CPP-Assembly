#include "../../project-base-v1.1/h/syscall_c.h"
#include "../../project-base-v1.1/h/Riscv.hpp"
#include "../../project-base-v1.1/h/TCB.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../../project-base-v1.1/h/MemoryAllocator.hpp"

int thread_id(){
    thread_dispatch();
    return TCB::running->ID;
}
void* mem_alloc (size_t size){
    if (size==0)
        return nullptr;
    size_t newsize=size+sizeof(BlockHeader);
    size_t abiSize=0;
    if((newsize) % MEM_BLOCK_SIZE)
        abiSize=(newsize)/MEM_BLOCK_SIZE + 1;
    else
        abiSize=(newsize)/MEM_BLOCK_SIZE;
    __asm__ volatile("mv a1,%0" : : "r"(abiSize));
    __asm__ volatile("li a0,0x1");
    __asm__ volatile("ecall");

    void* memoryptr;
    __asm__ volatile("mv %0, a0": "=r"(memoryptr));
    return memoryptr;
}

int mem_free (void* ptr){
    __asm__ volatile("mv a1,%0" : : "r"(ptr));
    __asm__ volatile("li a0,0x2");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0": "=r"(ret));
    return ret;
}

// THREADS

int thread_create (thread_t* handle,void(*start_routine)(void*),void* arg){
    void *stack=nullptr;
    void *sysstack=nullptr;
    if(start_routine != nullptr){
        stack = mem_alloc(sizeof(uint64)*DEFAULT_STACK_SIZE);
        sysstack = mem_alloc(sizeof(uint64)*DEFAULT_STACK_SIZE);
    }
    __asm__ volatile("mv a7,%0" : : "r"(sysstack));
    __asm__ volatile("mv a6,%0" : : "r"(stack));
    __asm__ volatile("mv a3,%0" : : "r"(arg));
    __asm__ volatile("mv a2,%0" : : "r"(start_routine));
    __asm__ volatile("mv a1,%0" : : "r"(handle));
    __asm__ volatile("li a0,0x11");
    __asm__ volatile("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (handle));
    if(handle != nullptr)
        return 0;
    else
        return -1;
};
/*
int thread_create_nostart (thread_t* handle,void(*start_routine)(void*),void* arg){
    void *stack=nullptr;
    void *sysstack=nullptr;
    if(start_routine != nullptr){
        stack = mem_alloc(sizeof(uint64)*DEFAULT_STACK_SIZE);
        sysstack = mem_alloc(sizeof(uint64)*DEFAULT_STACK_SIZE);
    }
    __asm__ volatile("mv a7,%0" : : "r"(sysstack));
    __asm__ volatile("mv a6,%0" : : "r"(stack));
    __asm__ volatile("mv a3,%0" : : "r"(arg));
    __asm__ volatile("mv a2,%0" : : "r"(start_routine));
    __asm__ volatile("mv a1,%0" : : "r"(handle));
    __asm__ volatile("li a0,0x15");
    __asm__ volatile("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (handle));
    if(handle != nullptr)
        return 0;
    else
        return -1;
}

int thread_start(thread_t handle){
    __asm__ volatile("mv a1,%0" : : "r"(handle));
    __asm__ volatile("li a0,0x16");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}*/

int thread_exit (){
    __asm__ volatile("li a0,0x12");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

void thread_dispatch (){
    __asm__ volatile("li a0,0x13");
    __asm__ volatile("ecall");
}

int sem_open (sem_t* handle,unsigned init){
    __asm__ volatile("mv a2,%0" : : "r"(init));
    __asm__ volatile("mv a1,%0" : : "r"(handle));
    __asm__ volatile("li a0,0x21");
    __asm__ volatile("ecall");

    __asm__ volatile ("mv %0, a0" : "=r" (handle));
    if(handle != nullptr)
        return 0;
    else
        return -1;
}

int sem_close (sem_t handle){
    __asm__ volatile("mv a1,%0" : : "r"(handle));
    __asm__ volatile("li a0,0x22");
    __asm__ volatile("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_wait (sem_t id){
    __asm__ volatile("mv a1,%0" : : "r"(id));
    __asm__ volatile("li a0,0x23");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_signal (sem_t id){
    __asm__ volatile("mv a1,%0" : : "r"(id));
    __asm__ volatile("li a0,0x24");
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

char getc (){
    //return __getc();
    __asm__ volatile("li a0,0x41");
    __asm__ volatile("ecall");

    char ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

void putc (char chr) {
    //__putc(chr);
    __asm__ volatile("mv a1,%0" : : "r"(chr));
    __asm__ volatile("li a0,0x42");
    __asm__ volatile("ecall");
}















