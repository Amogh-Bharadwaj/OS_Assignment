// TO COMPILE: gcc -pthread ThreadTesting.c -o sandbox. 
// Ignore the warnings, and run ./sandbox 

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


char* shared_memory;

union void_cast {
    void* ptr;
    int value;
};

int VOID_TO_INT(void* ptr) {
    union void_cast u;
    u.ptr = ptr;
    return u.value;
}

void* INT_TO_VOID(int value) {
    union void_cast u;
    u.value = value;
    return u.ptr;
}
 
void* execution_function(void* argument){
    
    int arg = VOID_TO_INT(argument);
    printf("Enter number of values when execution thread isn't sleeping:\n");
    int n;
    scanf("%d",&n);

    for(int i=0;i<n;i++){ 
        
        
        while(shared_memory!="Execution wake up"){
            printf("[EXECUTION THREAD]: Locked by monitor...\n");
            printf("Shared memory inside the while loop: %s\n",shared_memory);

            sleep(1);
        }
          
        printf("Shared memory after the while loop: %s\n",shared_memory);
                //Exits while loop when flag is 1 and enters critical section.
        printf("[EXECUTION THREAD]: Unlocked by monitor...\n");
        
        //Critical section
        /*int x;
        scanf("%d",&x);
        arg += x;  */
        printf("[EXECUTION THREAD]: Executing.\n");
           
    }
    printf("[EXECUTION THREAD] SUM: %d\n",arg);
    shared_memory="Die,monitor";
    

    //[TO BE FIXED] Putting flag to -1 will make monitor function exit
    //the while loop, but it doesn't exit.
}

void* monitor_function(){
    while(shared_memory!="Die,monitor"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(3);
        printf("[MONITOR THREAD]: Locking execution thread.\n");
        
        int shmid = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        shared_memory=(char*) shmat(shmid,(void*)0,0);
        shared_memory="Execution go to sleep";
        printf("Shared memory: %s",shared_memory);
        sleep(3);
        printf("[MONITOR THREAD]: Unlocking execution thread.\n");
        
        shared_memory="Execution wake up";
        printf("Shared memory: %s\n",shared_memory);
        sleep(1);
    }
    printf("[MONITOR THREAD] I'm done.");
}

int main(){
    pthread_t monitor_thread;
	pthread_t execution_thread;
    
    long sum = 0;

	//Concurrent execution of both threads
	pthread_create(&monitor_thread , NULL, monitor_function,NULL);
    pthread_create(&execution_thread , NULL, execution_function,INT_TO_VOID(sum));

	//pthread_join waits for the threads passed as argument to finish(terminate).
    pthread_join(execution_thread , NULL);
    pthread_join(monitor_thread, NULL);
    printf("\nMonitor ended.");
}