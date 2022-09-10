
#include "../../project-base-v1.1/h/syscall_c.h"
#include "../../project-base-v1.1/h/syscall_cpp.hpp"
#include "../../project-base-v1.1/h/TCB.hpp"
#include "../../project-base-v1.1/h/Print.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"
#include "../lib/mem.h"

int Thread::staticID=0;

void* operator new (size_t size){
    return mem_alloc(size);
};
void operator delete (void* ptr) {
    mem_free(ptr);
    return;
};

int Thread::getThreadId() {
    return thread_id();
}

Thread::Thread (void (*body)(void*), void* arg){
    //thread_create_nostart(&(this->myHandle),body,arg);
    ID=staticID++;
};

Thread::Thread (){
    myHandle=nullptr;
    ID=staticID++;
}

Thread::~Thread (){
    //thread_exit();
};

void Thread::ThreadWrapper(void* thread){
    Thread *t = (Thread*)thread;
    t->run();
}
void Thread::ThreadWrapper2(void* thread){
    printStringUser("gas\n");
    printStringUser("gas\n");
    printStringUser("gas\n");
    printStringUser("gas\n");
}

int Thread::start () {
    //return thread_start(myHandle);
    if (myHandle==nullptr)
        return thread_create(&myHandle,Thread::ThreadWrapper,this);
    return 0;
}
void Thread::dispatch (){
    thread_dispatch();
}

int Thread::sleep (time_t){
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
Semaphore::Semaphore (unsigned init){
    sem_open(&myHandle,init);
}
Semaphore::~Semaphore (){
    sem_close(myHandle);
}
int Semaphore::wait (){
    return sem_wait(myHandle);
}
int Semaphore::signal (){
    return sem_signal(myHandle);
}
//----------------------------------------------------------------------------------------------------------------------
char Console::getc (){
    return ::getc();
}
void Console::putc (char c){
    ::putc(c);
}
