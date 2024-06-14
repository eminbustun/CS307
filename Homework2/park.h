#ifndef GARAGE_H
#define GARAGE_H

#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <pthread.h>
#include <atomic>

using namespace std;

class Garage {
private:
    mutex mtx;
    unordered_map<pthread_t, condition_variable*> cv_map;
    atomic<int> fast;

public:
    Garage() = default;
    ~Garage() {
        for (auto &pair : cv_map) {
            delete pair.second;
        }
    }

    void park() {
        unique_lock<mutex> lock(mtx);
        auto current_id = pthread_self();
        if (cv_map.find(current_id) == cv_map.end()) {
            cv_map[current_id] = new condition_variable;
        }
        condition_variable *cv = cv_map[current_id];
        cv->wait(lock);
    }

    void unpark(pthread_t id) {
        lock_guard<mutex> lock(mtx);
        auto it = cv_map.find(id);
        if (it != cv_map.end()) {
            condition_variable *cv = it->second;
            cv->notify_one();
        }
    }
};

#endif