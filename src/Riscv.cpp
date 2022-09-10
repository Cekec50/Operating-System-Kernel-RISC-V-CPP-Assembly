//
// Created by marko on 20.4.22..
//

#include "../../project-base-v1.1/h/Riscv.hpp"
#include "../../project-base-v1.1/h/TCB.hpp"
#include "../lib/console.h"
#include "../../project-base-v1.1/h/MemoryAllocator.hpp"
#include "../../project-base-v1.1/h/Print.hpp"
#include "../../project-base-v1.1/h/syscall_c.h"
#include "../../project-base-v1.1/h/SemaphoreManager.hpp"
#include "../../project-base-v1.1/h/Timer.hpp"



void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::checkRunning(){
    uint64 data1 = MemoryAllocator::createdMainThread;
    __asm__ volatile("mv t0,%0" : : "r"(data1));
}

void Riscv::checkRunningStack(){
    uint64* data1 = (uint64*)(TCB::running->context.ssp_interrupt);
    __asm__ volatile("mv t0,%0" : : "r"(data1));
}

void Riscv::returnContext(){
    uint64* data1 = (uint64*)(&TCB::running->context.ssp_interrupt);
    __asm__ volatile("mv t0,%0" : : "r"(data1));
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();

    switch(scause){
        case(0x0000000000000008UL):case(0x0000000000000009UL):{
            //interrupt: no;  cause code: environment call from U-mode(8) or S-mode(9)
            uint64 opcode;
            __asm__ volatile("mv %0, a0": "=r"(opcode));
            switch(opcode){
                case(0x1):{ // "mem_alloc"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    size_t size;
                    __asm__ volatile("mv %0, a1": "=r"(size));
                    void* freemem = MemoryAllocator::allocate(size*MEM_BLOCK_SIZE);
                    __asm__ volatile("mv a0,%0" : : "r"(freemem));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x2):{ // "mem_free"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    void* ptr;
                    __asm__ volatile("mv %0, a1": "=r"(ptr));
                    int ret = MemoryAllocator::deallocate((char*)ptr);
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x4): {
                    mc_sstatus(SSTATUS_SPP);
                    uint64 sepc = r_sepc()+4;
                    w_sepc(sepc);
                    break;
                }
                case(0x11):{ // "thread_create"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    thread_t* handle;
                    void(*start_routine)(void*);
                    void* arg;
                    void* stack;
                    void* sysstack;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    __asm__ volatile("mv %0, a2": "=r"(start_routine));
                    __asm__ volatile("mv %0, a3": "=r"(arg));
                    __asm__ volatile("mv %0, a6": "=r"(stack));
                    __asm__ volatile("mv %0, a7": "=r"(sysstack));
                    *handle=TCB::createThread(start_routine,arg,(uint64*)stack,(uint64*)sysstack);
                    __asm__ volatile("mv a0,%0" : : "r"(handle));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x12):{ // "thread_exit"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    int ret=TCB::exit();
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x13):{ // "thread_dispatch"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    TCB::dispatch();
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x14):{ // yield
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    TCB::timeSliceCounter = 0;
                    TCB::dispatch();
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                /*case(0x15):{ // "thread_create_nostart"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    thread_t* handle;
                    void(*start_routine)(void*);
                    void* arg;
                    void* stack;
                    void* sysstack;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    __asm__ volatile("mv %0, a2": "=r"(start_routine));
                    __asm__ volatile("mv %0, a3": "=r"(arg));
                    __asm__ volatile("mv %0, a6": "=r"(stack));
                    __asm__ volatile("mv %0, a7": "=r"(sysstack));
                    *handle=TCB::createThread_nostart(start_routine,arg,(uint64*)stack,(uint64*)sysstack);
                    __asm__ volatile("mv a0,%0" : : "r"(handle));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }*/
                case(0x16):{ // "thread_start"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    thread_t handle;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    int ret=handle->start();
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x21):{ // "sem_open"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    sem_t* handle;
                    unsigned init;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    __asm__ volatile("mv %0, a2": "=r"(init));
                    *handle=SemaphoreManager::open(handle,init);
                    __asm__ volatile("mv a0,%0" : : "r"(handle));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x22):{ // "sem_close"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    sem_t handle;
                    int ret;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    if(!handle)
                        ret=-1;
                    else
                        ret=handle->SemaphoreManager::close();
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x23):{ // "sem_wait"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    sem_t handle;
                    int ret;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    if(!handle)
                        ret=-1;
                    else
                        ret=handle->SemaphoreManager::wait();
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x24):{ // "sem_signal"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    sem_t handle;
                    int ret;
                    __asm__ volatile("mv %0, a1": "=r"(handle));
                    if(!handle)
                        ret=-1;
                    else
                        ret=handle->SemaphoreManager::signal();
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x41):{ // "getc"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    __asm__ volatile("li t2,500");
                    char ret = __getc();
                    __asm__ volatile("li t2,600");
                    //w_scause(9);
                    __asm__ volatile("mv a0,%0" : : "r"(ret));
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
                case(0x42):{ // "putc"
                    uint64 sepc = r_sepc() + 4;
                    uint64 sstatus = r_sstatus();
                    char chr;
                    __asm__ volatile("mv %0, a1": "=r"(chr));
                    __putc(chr);
                    w_sstatus(sstatus);
                    w_sepc(sepc);
                    break;
                }
            }
            break;
        }
        case(0x8000000000000001UL):{
            // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
/*
            if(Timer::time % 500 == 0){

                printStringUser("Time: ");
                printIntegerUser(Timer::time);
                printStringUser(" ms \n");
                TCB::globalSemaphore->signal();
            }
            Timer::time+=100;*/
            mc_sip(SIP_SSIE);
            break;
        }
        case(0x8000000000000009UL):{
            // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
            console_handler();
            break;
        }
        default:{
            // unexpected trap cause
            uint64 scause = r_scause();
            uint64 sepc = r_sepc();
            printStringUser("Unexpected error!\n");
            printStringUser("scause: ");
            printIntegerUser(scause);
            printStringUser("\n");
            printStringUser("sepc: ");
            printIntegerUser(sepc);
            printStringUser("\n");
            break;
        }
    }
}
void Riscv::switchToSysStack() {
/*

    if((TCB::running == nullptr)){
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile("li t6,0");
        return;
        //printString("Nije promenjeno!");
    }
    if ((TCB::running->sys_stack != nullptr)&&(TCB::running->context.ssp_interrupt != 0)) {
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile ("sd %0, 2*8(sp)" : "=r" (TCB::running->context.ssp_interrupt));
        //__asm__ volatile("li t1,10");
         //printString("Promenjeno!");
    } else {
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile("li t6,0");
        //printString("Nije promenjeno!");
    }*/
/*
    if(TCB::running==nullptr)
        return;
    if(TCB::running->sys_stack != nullptr){
        __asm__ volatile("addi sp, sp, +16");
        __asm__ volatile("mv %0, sp": "=r"(TCB::running->context.sp));
        __asm__ volatile("mv sp,%0" : : "r"(TCB::running->context.ssp_interrupt));
        __asm__ volatile("addi sp, sp, -16");
        //printString("Promenjeno na sistemski!");
    }*/
    return;
}
void Riscv::switchFromSysStack(){

/*
    if((TCB::running == nullptr)){
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile("li t6,0");
        return;
        //printString("Nije promenjeno!");
    }*//*
    if ((TCB::running->sys_stack != nullptr)&&(TCB::running->context.ssp_interrupt != 0)) {
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile ("sd %0, 2*8(sp)" : "=r" (TCB::running->context.ssp_interrupt));
        //__asm__ volatile("li t1,10");
        //printString("Promenjeno!");
    } else {
        uint64* data1 = (uint64*)(&TCB::running);
        __asm__ volatile("mv %0, t0": "=r"(data1));
        //__asm__ volatile("li t6,0");
        //printString("Nije promenjeno!");
    }*/

    /*
    if(TCB::running==nullptr)
        return;
    if(TCB::running->sys_stack != nullptr) {
        __asm__ volatile("addi sp, sp, +16");
        __asm__ volatile("mv %0, sp": "=r"(TCB::running->context.ssp_interrupt));
        __asm__ volatile("mv sp,%0" : : "r"(TCB::running->context.sp_interrupt));
        __asm__ volatile("addi sp, sp, -16");
        //printString("Vraceno sa sistemskog!");
    }*/
    return;
}
