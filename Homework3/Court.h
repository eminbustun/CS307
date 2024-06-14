#ifndef BASKETBALLCOURT_H
#define BASKETBALLCOURT_H

#include <iostream>
#include <pthread.h>
#include <atomic>
#include <stdexcept>
#include <semaphore.h> 


using namespace std;


class Court {

    public:

        Court(int num_players, int referee_exist):
            number_of_players(num_players), referee_exist(referee_exist){
                
                if (num_players <= 0){
                    throw runtime_error("An error occured.");
                    
                }

                if (referee_exist != 0 && referee_exist != 1){
                    throw runtime_error("An error occured.");
                
                }
                total_people = number_of_players + referee_exist;
                sem_init(&match_played, 0, total_people);

                pthread_barrier_init(&barrier,NULL, total_people );
                people_in_court = 0;
                counter = 0;
                match_exist = false;
                pthread_mutex_init(&mtx, NULL);
        }

        void enter(){

            printf("Thread ID: %ld, I have arrived at the court.\n", pthread_self());
           
            sem_wait(&match_played);
           
            pthread_mutex_lock(&mtx); 
            people_in_court = people_in_court + 1;
            pthread_mutex_unlock(&mtx); 

            
            if (referee_exist == 0){
                isRefereeLeft = false;
                if (people_in_court == total_people){
                    printf("Thread ID: %ld, There are enough players, starting a match.\n", pthread_self());
                    match_exist = true;
                }
                else {
                    printf("Thread ID: %ld, There are only %d players, passing some time.\n", pthread_self(), people_in_court.load());

                    }

                }
                else {
                    if (referee_exist == 1) {
                        if (people_in_court == total_people){
                            match_exist = true;
                            printf("Thread ID: %ld, There are enough players, starting a match.\n", pthread_self());

                            referee = pthread_self();
                        }
                        else {
                            printf("Thread ID: %ld, There are only %d players, passing some time.\n", pthread_self(), people_in_court.load());

                        }

                    }
                }
            
            
            
        }

        void play();

        void leave(){

            if (match_exist == false){
                printf("Thread ID: %ld, I was not able to find a match and I have to leave.\n", pthread_self());
                people_in_court--;
                sem_post(&match_played);
                return;
            }


            pthread_mutex_lock(&mtx);
            if (pthread_equal(referee, pthread_self())) {
                printf("Thread ID: %ld, I am the referee and now, match is over. I am leaving.\n", pthread_self());
                
                isRefereeLeft = true;
                pthread_mutex_lock(&refLock);
                
                people_in_court--;
                pthread_mutex_unlock(&refLock);

                //printf("Thread ID: %ld, people in the court = %d.\n", pthread_self(), people_in_court.load());

                pthread_mutex_unlock(&mtx);
                pthread_barrier_wait(&barrier);
                
                
                sem_post(&match_played);

                //sem_post(&referee_gone); // Signal referee has left
                
                return;
            }
            pthread_mutex_unlock(&mtx);
        

            pthread_barrier_wait(&barrier); // Everyone waits at the barrier before leaving
            pthread_mutex_lock(&mtx);
            printf("Thread ID: %ld, I am a player and now, I am leaving.\n", pthread_self());
            people_in_court--;
            //printf("Thread ID: %ld, people in the court = %d.\n", pthread_self(), people_in_court.load());

            if (people_in_court == 0) {
                match_exist == false;
                printf("Thread ID: %ld, everybody left, letting any waiting people know.\n", pthread_self());
                sem_post(&match_played);
                

                pthread_barrier_destroy(&barrier);
                pthread_barrier_init(&barrier, NULL, total_people);
                
                
                
            }
           
            
            
           
            pthread_mutex_unlock(&mtx);

           
        }

    private:
        sem_t match_played; 
        int total_people;
        int number_of_players;
        int referee_exist; // 1: referee exist; 0: referee does not exist
        pthread_t referee;
        atomic<int> isRefereeLeft;
        atomic<bool> match_exist;
        atomic<int> people_in_court;
        pthread_barrier_t barrier; 
        atomic<int> counter;
        pthread_mutex_t mtx, refLock; 
};

#endif // BASKETBALLCOURT_H