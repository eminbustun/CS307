#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <pthread.h>
#include <cassert>

using namespace std;

template <typename T>
class Queue {
    
    public:
        Queue() {
            q.head = q.tail = nullptr;
            pthread_mutex_init(&(q.head_lock), NULL);
            pthread_mutex_init(&(q.tail_lock), NULL);
        }

        void enqueue(T item) {
            Node * tmp = new Node();
            //assert(tmp != NULL);
            tmp->value = item;
            tmp->next = nullptr;

            pthread_mutex_lock(&q.tail_lock);
            if (q.head == nullptr) {
                q.head = q.tail = tmp;
            } else {
                q.tail->next = tmp;
                q.tail = tmp;
            }
            pthread_mutex_unlock(&q.tail_lock);
        }

       T dequeue(){
            pthread_mutex_lock(&q.head_lock);
            if (q.head == nullptr) {
                pthread_mutex_unlock(&q.head_lock);
                return NULL;  
            }
            Node *tmp = q.head;
            T value = tmp->value;
            Node *new_head = tmp->next;
            q.head = new_head;
            if (q.head == nullptr) {
                q.tail = nullptr;
            }
            pthread_mutex_unlock(&q.head_lock);
            delete tmp; 
            return value;
        }

        bool isEmpty() {
            pthread_mutex_lock(&q.head_lock);
            bool empty = (q.head == nullptr);
            pthread_mutex_unlock(&q.head_lock);
            return empty;
        }   

        void print() {
            pthread_mutex_lock(&q.head_lock);
            Node* current = q.head;
            pthread_mutex_unlock(&q.head_lock);
            
            while (current != nullptr) {
                std::cout << current->value << " ";
                current = current->next;
            }


            std::cout << std::endl;
    }

    private:

        struct Node {
            T value;
            Node *next;
        };

        struct queue {
            Node *head;
            Node *tail;
            pthread_mutex_t head_lock, tail_lock;
        };

        queue q;

};

#endif