//
// Created by os on 5/29/22.
//

#ifndef PROJECT_BASE_V1_0_SCHEDULER_HPP
#define PROJECT_BASE_V1_0_SCHEDULER_HPP
#include "List.hpp"
class TCB;

class Scheduler
{
private:
    static List<TCB> readyThreadQueue;

public:
    static TCB *get();

    static void put(TCB *tcb);

};
#endif //PROJECT_BASE_V1_0_SCHEDULER_HPP
