//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "Scheduler.hpp"
#include "SemaphoreManager.hpp"

// Thread Control Block
class SemaphoreManager;
class TCB
{
public:
    static void initializeGlobalSemaphore();
    static SemaphoreManager* globalSemaphore;

    static int staticID;

    int ID=staticID++;

    void *operator new(size_t size) { return MemoryAllocator::before(size); }

    void operator delete(void *ptr) { MemoryAllocator::deallocate((char*)ptr);}

    void *operator new[](size_t n) { return MemoryAllocator::before(n);}

    void operator delete[](void *p) noexcept { MemoryAllocator::deallocate((char*)p);}

    ~TCB() { delete stack; delete sys_stack; }

    bool isFinished() const { return finished; }

    bool isBlocked() { return blocked;}

    void setFinished(bool value) { finished = value; }

    void setBlocked(bool value) { blocked=value;}

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body,void* arguments,uint64* stack, uint64* sysstack);

    //static TCB *createThread_nostart(Body body,void* arguments,uint64* stack, uint64* sysstack);

    static void yield();

    int start();

    static int exit();

    static TCB *running;

    TCB(){}

private:
    TCB(Body body,void* arguments, uint64* stack, uint64* sysstack,uint64 timeSlice=DEFAULT_TIME_SLICE) :
            body(body),
            arguments(arguments),
            stack(body != nullptr ? stack : nullptr),
            sys_stack(body != nullptr ? sysstack : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0,
                     sys_stack != nullptr ? (uint64) &sys_stack[DEFAULT_STACK_SIZE] : 0,
                     0
                    }),
            timeSlice(timeSlice),
            finished(false),
            blocked(false)
    {
        //if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
        uint64 ssp_interrupt;
        uint64 sp_interrupt;
    };

    Body body;
    void* arguments;
    uint64 *stack;
    uint64 *sys_stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool blocked;

    friend class Riscv;

    friend class SemaphoreManager;

    friend class Thread;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static void block();

    static uint64 timeSliceCounter;

    //static uint64 constexpr STACK_SIZE = 1024;
    //static uint64 constexpr TIME_SLICE = 2;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
