#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int play=0;

void* task_thread(){
    for(int i=0;i<10;i++){
        //pthread_mutex_lock(&mutex);
        while(!play){
            pthread_cond_wait(&cond,&mutex);

            pthread_mutex_unlock(&mutex);
        }
        printf("%d\n",i);
        //sleep(1);
    }
}

void* monitor_thread(){
    //start task
    //sleep(3)
    //stop task
    //sleep(3)
    //start
    
    printf("[MONITOR]: Playing \n");
    pthread_mutex_lock(&mutex);
    play=1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    printf("[MONITOR]: Pausing \n");
    pthread_mutex_lock(&mutex);
    play=0;
    pthread_mutex_unlock(&mutex);

    printf("[MONITOR]: Sleeping \n");
    sleep(3);

    printf("[MONITOR]: Playing \n");
    pthread_mutex_lock(&mutex);
    play=1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    
    
}

int main(){
    pthread_t tt,mt;
    
    pthread_create(&mt,NULL,monitor_thread,NULL);
    pthread_create(&tt,NULL,task_thread,NULL);

    pthread_join(tt,NULL);
    pthread_join(mt,NULL);
    
}