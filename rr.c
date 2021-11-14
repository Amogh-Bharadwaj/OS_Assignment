// FCFS Code
// TO BE FIXED: Critical section being skipped [Not performing input of numbers] and C1 not terminating.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>

/* 
 pthread_wait_cond will automatically release a lock before it and wait on a conditional variable
 until signalled. 
 We use this to communicate between threads.
*/

pthread_cond_t T1=PTHREAD_COND_INITIALIZER,T2=PTHREAD_COND_INITIALIZER,T3=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int play1=0,play2=0,play3=0;

void* share_memory(size_t size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

char* C1_memory; // Interthread communication (monitor thread of C1 to execution thread of C1).
char* C2_memory;
char* C3_memory;

char* MC1_memory; // This is for Main to the three processes.
char* MC2_memory;
char* MC3_memory;

char* D1;
char* D2;
char* D3;

//Function to convert string to integer, or sti for short. 
int sti(char s[]){
    int num = 0;
    int l = strlen(s);
   
    for(int i=0;i<l-1;i++){
        num+=(int)(s[i]-'0');
        num*=10;
    }
    
    num=num/10;
    return num;
}

void* C3_execution_function(void *arg){  
    
    // Opening n3 text file.
    FILE* fp;
    fp = fopen("sample.txt" , "r");
    char str[8];
	long sum=0;

    while(fgets(str,10,fp)!=NULL){
        // C3 is asleep until monitor tells to wake up.
		pthread_mutex_lock(&mutex);
        while(!play3){pthread_cond_wait(&T3,&mutex);}
        
        // Critical section
        printf("[C3]: Adding..\n");
        sum += atoi(str);
        pthread_mutex_unlock(&mutex);
    }
    
    //strcpy(MC3_memory,"Die");
    //printf("Reached here. MC3_Memory: %s\n",MC3_memory);

    return (void *)sum;
}

void* C3_monitor_function(void *arg){
     
    while(strcmp(D3,"Die")!=0){ 
        // If scheduler says to stop.
        if(strcmp(MC3_memory,"Stop")==0){
            //printf("Locking...\n");

            // To pause
            pthread_mutex_lock(&mutex);
            play3 = 0;
            pthread_mutex_unlock(&mutex);
        }
      
        // If scheduler says to start.
        if(strcmp(MC3_memory,"Start")==0){

            // To play
            pthread_mutex_lock(&mutex);
            play3 = 1;
            pthread_cond_signal(&T3);
            pthread_mutex_unlock(&mutex);
        }
      
    }

}

void* C2_execution_function(void *arg)
{   
    // Opening n2 file.
    FILE* fp1;
    fp1 = fopen("sample.txt","r");
    char str[8];
    
	while(fgets(str,10,fp1)!=NULL){
        pthread_mutex_lock(&mutex);
        while(!play2){pthread_cond_wait(&T2,&mutex);}
            
        //Critical section
        int num = atoi(str);
        printf("[C2]: %d\n" , num);
        pthread_mutex_unlock(&mutex);
    }
    
    // Intimating to monitor that C2 is over.
	//strcpy(D2,"Die");
}

void* C2_monitor_function(void *arg){

    sleep(1);
    while(strcmp(D2,"Die")!=0){ 

        if(MC2_memory=="Die"){
            //printf("[C1 MONITOR]: Im out\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        // If scheduler says to stop.
        if((strcmp(MC2_memory,"Stop")==0)){
            //printf("Locking C2...\n");
            pthread_mutex_lock(&mutex);
            play2 = 0;
            pthread_mutex_unlock(&mutex);
        }
        
     
        // If scheduler says to start.
        if(strcmp(MC2_memory,"Start")==0){
            pthread_mutex_lock(&mutex);
            play2 = 1;
            pthread_cond_signal(&T2);
            pthread_mutex_unlock(&mutex);
        }
      
    }
}


void* C1_execution_function(void* argument){

    long long arg = 0;
    printf("[C1]: Enter number of values when execution thread isn't sleeping:\n");
    
    int n;
    scanf("%d",&n); 
    int arr[n];
    for(int i=0;i<n;i++)arr[i]=(rand()%1000000);
    for(int i=0;i<n;i++){ 
        // Unless monitor tells me to start, I will be asleep.
        
        pthread_mutex_lock(&mutex);
        while(!play1){pthread_cond_wait(&T1,&mutex);}
        
        //Critical section
        printf("[C1]: Adding..\n");
        arg+=arr[i];  
        pthread_mutex_unlock(&mutex);
    }
    //printf("[C1]: SUM: %lld\n",arg);
   
    // Intimating to monitor that execution is over.
    
    return (void *)arg;
}

void* C1_monitor_function(){
    // If C1 is over, monitor thread should terminate.
   
    
    while(strcmp(D1,"Die")!=0){    

        //printf("MC1 memory: %s\n",MC1_memory);
        if(MC1_memory=="Die"){
            printf("[C1 MONITOR]: Im out\n");
            pthread_mutex_unlock(&mutex);
            break;
        }

       
        // If scheduler says to stop.
        if(strcmp(MC1_memory,"Stop")==0){
            //printf("Locking C1...\n");
            pthread_mutex_lock(&mutex);
            play1 = 0;
            pthread_mutex_unlock(&mutex);
        }
       
        // If scheduler says to start.
        if(strcmp(MC1_memory,"Start")==0){
            //printf("Unlocking..\n");
            pthread_mutex_lock(&mutex);
            play1 = 1;
            pthread_cond_signal(&T1);
            pthread_mutex_unlock(&mutex);
        }
        
    }
}

int main()
{   //Process PIDs.
	int pid,pid1,pid2;

    // Pipes.
    int p1[2],p2[2],p3[2];
    char buf[40];

    // List of shared memories. 
    // C1,C2,C3 are for monitor-to-execution thread communication.

   // MC1,MC2,MC3 are main-to-process communication
   MC1_memory = share_memory(128);
   strcpy(MC1_memory,"Stop");

   MC2_memory = share_memory(128);
   strcpy(MC2_memory,"Stop");

   MC3_memory = share_memory(128);
   strcpy(MC3_memory,"Stop");

   // To kill the threads.
   D1 = share_memory(128);
   strcpy(D1,"Live");

   D2 = share_memory(128);
   strcpy(D2,"Live");

   D3 = share_memory(128);
   strcpy(D3,"Live");


   clock_t C1_Arrival,C2_Arrival,C3_Arrival;
   double C1_Arrival_Time,C2_Arrival_Time,C3_Arrival_Time;
    
    
    //Creating pipes.
    if (pipe(p1)==-1)
	{
		fprintf(stderr, "Pipe Failed" );
		return 1;
	}
	if (pipe(p2)==-1)
	{
		fprintf(stderr, "Pipe Failed" );
		return 1;
	}
	if (pipe(p3)==-1)
	{
		fprintf(stderr, "Pipe Failed" );
		return 1;
	}
 
 
	// variable pid will store the
	// value returned from fork() system call
    C1_Arrival = clock();
    C1_Arrival_Time = (C1_Arrival/CLOCKS_PER_SEC)*1000;
    
	pid = fork();
 
	// If fork() returns zero then it
	// means it is child process.
	if (pid == 0) {

        // C1
        //strcpy(MC1_memory,"Start");// Setting to start so C1 will start immediately.

        

		pthread_t C1_monitor_thread;
		pthread_t C1_execution_thread;

        void* status; // To store return value from execution thread.

        //Concurrent execution of both threads
        pthread_create(&C1_execution_thread , NULL, C1_execution_function,NULL);
        sleep(2);
		pthread_create(&C1_monitor_thread , NULL, C1_monitor_function,NULL);
    	

		//pthread_join waits for the threads passed as argument to finish(terminate).
    	pthread_join(C1_execution_thread , &status); //The value returned by the execution function will be stored in status.
    	//pthread_join(C1_monitor_thread, NULL); 

        long sum = (long)status; // Type casting status to long and storing in sum.

        //printf("Sum inside C1 child: %ld\n",sum);
        //printf("------------------------------------------------\n");

        close(p1[0]);
        write(p1[1],&sum,sizeof(sum)); // Writing sum to pipe.
        close(p1[1]);
        strcpy(D1,"Die");
       

    }

        else {
        //wait(NULL);
        C2_Arrival = clock();
        C2_Arrival_Time = (C2_Arrival/CLOCKS_PER_SEC)*1000;


		pid1 = fork();
		if (pid1 == 0) {
            //C2
   
			pthread_t C2_execution_thread;
            sleep(1);
            pthread_t C2_monitor_thread;

			//Concurrent execution of both threads
			pthread_create(&C2_monitor_thread , NULL, C2_monitor_function,NULL);
    		pthread_create(&C2_execution_thread , NULL, C2_execution_function,NULL);

            //pthread_join waits for the threads passed as argument to finish(terminate).
    		pthread_join(C2_execution_thread , NULL);
    		//pthread_join(C2_monitor_thread, NULL);
            close(p2[0]);
            write(p2[1],"Done Printing",14);
            close(p2[1]);
            strcpy(D2,"Die");
           
		}
		else {
            //wait(NULL);
            
            C3_Arrival = clock();
            C3_Arrival_Time = (C3_Arrival/CLOCKS_PER_SEC)*1000;

			pid2 = fork();
			if (pid2 == 0) {               

				pthread_t C3_monitor_thread;
				pthread_t C3_execution_thread;
                
                void* status;

                //Concurrent execution of both threads
				
    			pthread_create(&C3_execution_thread , NULL, C3_execution_function,NULL);
                pthread_create(&C3_monitor_thread , NULL, C3_monitor_function,NULL);

                pthread_join(C3_execution_thread , &status);
                long sum2 = (long)status;
    		    //pthread_join(C3_monitor_thread, NULL);

                close(p3[0]);
                write(p3[1],&sum2,sizeof(sum2));
                close(p3[1]);
                strcpy(D3,"Die");
            }
            else{

                //wait(NULL);
                int c1_sum,c3_sum;

                while(1){
                    
                   
                    if(strcmp(D1,"Die")!=0){
                        strcpy(MC1_memory,"Start");
                        sleep(2);
                        strcpy(MC1_memory,"Stop");
                    }
                
                     if(strcmp(D2,"Die")!=0){
                        strcpy(MC2_memory,"Start");
                        sleep(2);
                        strcpy(MC2_memory,"Stop");
                    }
                  

                     if(strcmp(D3,"Die")!=0){
                        strcpy(MC3_memory,"Start");
                        sleep(2);
                        strcpy(MC3_memory,"Stop");
                    }
                   

                    if((strcmp(D1,"Die")==0) && (strcmp(D2,"Die")==0) && (strcmp(D3,"Die")==0))break;
                }
                    
                       
                        // Getting message via pipe from C1.
                        read(p1[0],&c1_sum,sizeof(c1_sum));
                        close(p1[0]);
                        printf("C1 output: %d\n",c1_sum);

                        // Getting message via pipe from C2.
                        read(p2[0],buf,14);
                        close(p2[0]);
                        printf("C2 output: %s\n",buf);

                        // Getting message via pipe from C3.
                        read(p3[0],&c3_sum,sizeof(c3_sum));
                        close(p3[0]); 
                        printf("C3 output: %d\n",c3_sum);
            }
             
        }
    }
}