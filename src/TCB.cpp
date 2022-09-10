//
// Created by marko on 20.4.22..
//

#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/Print.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;
int TCB::staticID=0;

SemaphoreManager* TCB::globalSemaphore=nullptr;

void TCB::initializeGlobalSemaphore() {
    TCB::globalSemaphore = new SemaphoreManager(&TCB::globalSemaphore,0);
}

// creates a thread and puts in the Scheduler
TCB *TCB::createThread(Body body,void* arguments,uint64* stack,uint64* sysstack)
{
    TCB* tcb=new TCB(body,arguments, stack,sysstack,DEFAULT_TIME_SLICE);
    if (body != nullptr) { Scheduler::put(tcb);}
    return tcb;
}
/*
// creates a thread
TCB *TCB::createThread_nostart(Body body,void* arguments,uint64* stack,uint64* sysstack)
{
    TCB* tcb=new TCB(body,arguments, stack,sysstack,DEFAULT_TIME_SLICE);
    return tcb;
}*/

int TCB::start(){
    Scheduler::put(this);
    return 0;
}

void TCB::yield()
{
    __asm__ volatile("li a0,0x14");
    __asm__ volatile ("ecall");
}
// dispatch the current thread (and put it in the Scheduler)
void TCB::dispatch()
{
    TCB::timeSliceCounter = 0;
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();
    if(running==0) printStringUser("NEMA NISTA U SCHEDULERU!");

    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    TCB::contextSwitch(&old->context, &running->context);
}
//dispatch the current thread (doesn't put it in the Scheduler)
void TCB::block(){
    TCB::timeSliceCounter = 0;
    TCB *old = running;
    running = Scheduler::get();
    if(running==0) printStringUser("NEMA NISTA U SCHEDULERU!");

    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arguments);
    thread_exit();
}

int TCB::exit() {
    if(TCB::running->finished==true)
        return -1;
    running->setFinished(true);
    TCB::dispatch();
    return 0;
}


