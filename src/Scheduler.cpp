#include "../../project-base-v1.1/h/Scheduler.hpp"

List<TCB> Scheduler::readyThreadQueue;

TCB *Scheduler::get(){
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB *tcb){
    readyThreadQueue.addLast(tcb);
}