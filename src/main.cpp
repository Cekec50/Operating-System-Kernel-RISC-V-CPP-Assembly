


//#include "../lib/mem.h"/

#include "../../project-base-v1.1/h/syscall_cpp.hpp"
#include "../../project-base-v1.1/h/syscall_c.h"
#include "../../project-base-v1.1/h/Riscv.hpp"
#include "../../project-base-v1.1/h/MemoryAllocator.hpp"
#include "../../project-base-v1.1/h/TCB.hpp"
#include "../../project-base-v1.1/h/SemaphoreManager.hpp"
#include "../h/userMain.hpp"
#include "../h/Timer.hpp"

void userMainWrapper(void* sem1){
    userMain();
    sem_signal((sem_t)sem1);
}
void dispatcherThread(void*){
    while(true){
        thread_dispatch();}
}
int main(){

    MemoryAllocator::initializeHead();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    TCB::initializeGlobalSemaphore();
    //Timer *timer=new Timer; // for the modification
    __asm__ volatile("li a0,0x4");
    __asm__ volatile("ecall");
    thread_t thread0;
    thread_create(&thread0,nullptr,nullptr);
    TCB::running=thread0;
    MemoryAllocator::createdMainThread=1;

    thread_t thread10;
    thread_create(&thread10,dispatcherThread,nullptr);


    sem_t userMainWait;
    sem_open(&userMainWait,0);

    thread_t thread1;
    thread_create(&thread1,userMainWrapper,userMainWait);
    sem_wait(userMainWait);

    delete thread0;
    delete thread1;
    delete userMainWait;
    delete thread10;



    return 0;
}