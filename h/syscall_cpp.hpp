#ifndef _syscall_cpp
#define _syscall_cpp


#include "syscall_c.h"

void* operator new (size_t);
void operator delete (void*);
class Thread {
public:
    static int getThreadId();
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
    static void ThreadWrapper(void* thread);
    static void ThreadWrapper2(void* thread);
protected:
    Thread ();
    virtual void run () {}
private:
    static int staticID;
    int ID;
    thread_t myHandle;
    static void setBody(Thread*);
    friend class TCB;
};
class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};
class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
};
class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif