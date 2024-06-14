#include <iostream>
#include <random>
#include <pthread.h>
#include <unistd.h>
#include <queue.h>
#include <sched.h>
using namespace std;

Queue<int> q;

void* enq(void* arg) { 
    long base = (long) arg;
    cout<< "Thread with base: "<<base<<" started."<<endl;
    for (int i = base; i < base+100; i++)
        q.enqueue(i);
    return NULL;

}


int main() {
    cout << "Hello, from main." << endl;
    pthread_t e1, e2;
    pthread_create(&e1, NULL, enq, (void*)0);
    pthread_create(&e2, NULL, enq, (void*)100);
    pthread_join(e1, NULL);
    pthread_join(e2, NULL);
    for (int i=0; i < 100; i++)
       int x = q.dequeue();
    cout<<"Threads terminated. Resulting queue state:"<<endl;
    q.print();
    return 0;
}
