#ifndef MLFQMUTEX_H
#define MLFQMUTEX_H

#include <iostream>
#include <pthread.h>
#include <vector>
#include <atomic>
#include <cmath>
#include <chrono>
#include "queue.h"
#include "park.h"
#include <sched.h>
#include <unordered_map>

using namespace std;

class MLFQMutex {
public:
    MLFQMutex(int priorityL, double quat) :
        priority_levels(priorityL), quantum_value(quat), flag(false) {
        allQueues.resize(priority_levels);
        for (int i = 0; i < priority_levels; i++) {
            allQueues[i] = Queue<pthread_t>(); 
        }
        guard.clear();
    }

    void lock() {
        
        while (guard.test_and_set()) {
            sched_yield();
        }
        
        if (!flag) {
            flag = true;
            start = chrono::high_resolution_clock::now();

            guard.clear();
            return;
        }

        pthread_t tid = pthread_self();
        if (thread_tracker.find(tid) == thread_tracker.end()) {
            thread_tracker[tid] = 0;  
        }
        int level = thread_tracker[tid];
        
        allQueues[level].enqueue(tid);
      
        printf("Adding thread with ID: %ld to level %d\n", tid, level);
        garage.setPark();
        guard.clear();
        garage.park();

        start = chrono::high_resolution_clock::now();
    }

    void unlock() {
        
        while (guard.test_and_set()) {
            sched_yield();
        }
        pthread_t tid = pthread_self();
        
        int level = 0;
        bool getIn = false;
        for (int i = 0; i < priority_levels; i++){

            if (!allQueues[i].isEmpty()){ 
                pthread_t id = allQueues[i].dequeue();
                garage.unpark(id);
                end = chrono::high_resolution_clock::now();
                double duration = chrono::duration_cast<chrono::duration<double>>(end - start).count();
                // MLFQ LOGIC START
                
                if (quantum_value < duration) {
                    int level = thread_tracker[tid]  + floor((double)(duration / quantum_value));

                    if (level > priority_levels - 1) {
                        level = priority_levels - 1;
                    }

                    thread_tracker[tid] = level;
                }

                getIn = true;
                guard.clear();
                break;

            }


        }
        
        if (getIn == false) { //* If every queue is empty, change the flag and clear the guard.
           
            flag = false;
            guard.clear(); 
        }

        
    }

    void print() {
        while (guard.test_and_set()) { //* I am not sure if this is needed but it is safer.
            sched_yield();
        }
        for (int i = 0; i < priority_levels; i++){
            if (allQueues[i].isEmpty()){
                printf("Level %d: Empty\n", i); 
            }
            else {
                printf("Level %d: ", i); allQueues[i].print();

            }
        }
        guard.clear();
    }

private:
    vector<Queue<pthread_t>> allQueues;
    atomic_flag guard;
    atomic<bool> flag;
    int priority_levels;
    double quantum_value;
    chrono::high_resolution_clock::time_point start, end;
    Garage garage;
    unordered_map<pthread_t, int> thread_tracker;

};

#endif // MLFQMUTEX_H
