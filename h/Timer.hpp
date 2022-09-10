//
// Created by os on 8/30/22.
//
#include "Print.hpp"
#ifndef PROJECT_BASE_V1_1_TIMER_HPP
#define PROJECT_BASE_V1_1_TIMER_HPP
class Timer{
public:
    static void printTime(){
        printStringUser("Time: ");
        printIntegerUser(Timer::time);
        printStringUser(" s \n");
    }
    static void incrementTime(double inc){
        Timer::time+=inc;
    }
    static int time;


};
#endif //PROJECT_BASE_V1_1_TIMER_HPP
