//
// Created by os on 8/17/22.
//

#include "../h/SemaphoreManager.hpp"
#include "../h/Print.hpp"

// creates a new Semaphore
SemaphoreManager* SemaphoreManager::open(sem_t *handle,int init){
    return new SemaphoreManager(handle,init);
}
// returns all blocked threads to Scheduler
int SemaphoreManager::close(){
   while (blocked.peekFirst() != 0) {
        TCB *t = blocked.removeFirst();
        if(t->isBlocked()==false) return -1;
        t->setBlocked(false);
        Scheduler::put(t);
    }
    return 0;
}

int SemaphoreManager::wait(){
    if(--val<0) return block();
    return 0;
}

int SemaphoreManager::signal(){
    if(++val<=0) return unblock();
    return 0;
}

int SemaphoreManager::block(){
    blocked.addLast(TCB::running);
    if(TCB::running->isBlocked())
        return -1;
    TCB::running->setBlocked(true);
    TCB::block();
    return 0;
}

int SemaphoreManager::unblock(){
    TCB *t = blocked.removeFirst();
    if(!(t->isBlocked()))
        return -1;
    t->setBlocked(false);
    Scheduler::put(t);
    return 0;
}