
// FCFS Code
// TO BE FIXED: Critical section being skipped [Not performing input of numbers] and C1 not terminating.

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
#include <time.h>
#include <sys/mman.h>

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
    sleep(1);
    // Opening n3 text file.
    FILE* fp;
    fp = fopen("n3.txt" , "r");
    char str[8];
	int sum=0;

    do {
        // C3 is asleep until monitor tells to wake up.
		while(strcmp(C3_memory,"Stop")==0);//printf("[C3]: Locked by monitor...\n");
        
        // Critical section
        sum += atoi(str);
    }while(fgets(str,10,fp)!=NULL);

    strcpy(C3_memory,"Die");

    return (void *)sum;
}

void* C3_monitor_function(void *arg){
     sleep(1);
    while(strcmp(C3_memory,"Die")!=0){ 
        // If scheduler says to stop.
        if((strcmp(MC3_memory,"Stop")==0) && (strcmp(C3_memory,"Die")!=0))strcpy(C3_memory,"Stop");
        else break;
     
        // If scheduler says to start.
        if(( strcmp(MC3_memory,"Start")) && (strcmp(C3_memory,"Die")!=0))strcpy(C3_memory,"Start");
        else break;
    }

}

void* C2_execution_function(void *arg)
{   
    sleep(1);
    // Opening n2 file.
    FILE* fp1;
    fp1 = fopen("n2.txt","r");
    char str[8];
	do{ 
        
		while(strcmp(C2_memory,"Stop")==0)printf("[C2]: Locked by monitor...\n");
            
        //Critical section
        int num = atoi(str);
        printf("[C2]: %d\n" , num);
      } while(fgets(str,10,fp1)!=NULL);

    // Intimating to monitor that C2 is over.
	strcpy(C2_memory,"Die");
}

void* C2_monitor_function(void *arg){

    sleep(1);
    while(strcmp(C2_memory,"Die")!=0){ 
        // If scheduler says to stop.
        if((strcmp(MC2_memory,"Stop")==0) && (strcmp(C2_memory,"Die")!=0))strcpy(C2_memory,"Stop");
        else break;
     
        // If scheduler says to start.
        if(( strcmp(MC2_memory,"Start")) && (strcmp(C2_memory,"Die")!=0))strcpy(C2_memory,"Start");
        else break;
    }
}


void* C1_execution_function(void* argument){
    
    while(strcmp(C1_memory,"Start")!=0)printf("C1 ready.\n");
    int arg = 0;
    printf("[C1]: Enter number of values when execution thread isn't sleeping:\n");
    
    int n;
    scanf("%d",&n); 
   
    for(int i=0;i<n;i++){ 
        printf("C1 memory length: %d\n", strlen(C1_memory));
        //while(!strlen(C1_memory))printf("Empty\n");
        // Unless monitor tells me to start, I will be asleep.
        while(strcmp(C1_memory,"Stop")==0){printf("[C1]: Locked by monitor...\n");}
        
        //Critical section
        int x;
        printf("[C1]: Enter a number: ");
        scanf("%d",&x);
        sleep(1);
        arg += x;  
    }
    printf("[C1]: SUM: %d\n",arg);
    
    // Intimating to monitor that execution is over.
    strcpy(C1_memory,"Die");
    return (void *)arg;
}

void* C1_monitor_function(){
    // If C1 is over, monitor thread should terminate.
    
    sleep(2);
    while(strcmp(C1_memory,"Die")!=0){ 
         printf("[C1 MONITOR THREAD]: hello\n");   
        // If scheduler says to stop.
        if(strcmp(C1_memory,"Die")!=0)if(strcmp(MC1_memory,"Stop")==0){printf("In here\n");strcpy(C1_memory,"Stop");}
        else break;

        // If scheduler says to start.
        if(strcmp(C1_memory,"Die")!=0)if(strcmp(MC1_memory,"Start")==0)strcpy(C1_memory,"Start");
        else break;

       
    }
}

int main()
{   //Process PIDs.
	int pid, pid1, pid2;

    // Pipes.
    int p1[2],p3[2];

    // List of shared memories. 
    // C1,C2,C3 are for monitor-to-execution thread communication.
   C1_memory = share_memory(128);
   strcpy(C1_memory,"Blank");
   C2_memory = share_memory(128);
   strcpy(C2_memory,"Blank");
   C3_memory = share_memory(128);
   strcpy(C3_memory,"Blank");

   // MC1,MC2,MC3 are main-to-process communication
   MC1_memory = share_memory(128);
   strcpy(MC1_memory,"Stop");

   MC2_memory = share_memory(128);
   strcpy(MC2_memory,"Stop");

   MC3_memory = share_memory(128);
   strcpy(MC3_memory,"Stop");
    
    
    //Creating pipes.
    if (pipe(p1)==-1)
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
	pid = fork();
 
	// If fork() returns zero then it
	// means it is child process.
	if (pid == 0) {

        // C1
        strcpy(MC1_memory,"Stop");

		pthread_t C1_monitor_thread;
		pthread_t C1_execution_thread;

        void* status; // To store return value from execution thread.

        //Concurrent execution of both threads
		pthread_create(&C1_monitor_thread , NULL, C1_monitor_function,NULL);
    	pthread_create(&C1_execution_thread , NULL, C1_execution_function,NULL);

		//pthread_join waits for the threads passed as argument to finish(terminate).
    	pthread_join(C1_execution_thread , &status); //The value returned by the execution function will be stored in status.
    	pthread_join(C1_monitor_thread, NULL); 

        int sum = (int)status; // Type casting status to int and storing in sum.

        //printf("Sum inside C1 child: %ld\n",sum);
        printf("------------------------------------------------\n");

        close(p1[0]);
        write(p1[1],&sum,sizeof(sum)); // Writing sum to pipe.
        close(p1[1]);
        sleep(2);

    }

        else {
        //wait(NULL);
		pid1 = fork();
		if (pid1 == 0) {
            strcpy(MC2_memory,"Stop");
            if(strcmp(MC2_memory,"Start")==0){

            pthread_t C2_monitor_thread;
			pthread_t C2_execution_thread;

			//Concurrent execution of both threads
			pthread_create(&C2_monitor_thread , NULL, C2_monitor_function,NULL);
    		pthread_create(&C2_execution_thread , NULL, C2_execution_function,NULL);

            //pthread_join waits for the threads passed as argument to finish(terminate).
    		pthread_join(C2_execution_thread , NULL);
    		pthread_join(C2_monitor_thread, NULL);
            printf("------------------------------------------------\n");
            }
            
		}
		else {
            //wait(NULL);
            
			pid2 = fork();
			if (pid2 == 0) {
                //C3

                strcpy(MC3_memory,"Stop");
              
            
				pthread_t C3_monitor_thread;
				pthread_t C3_execution_thread;
                

                void* status;

                if(strcmp(MC3_memory,"Start")==0){

                //Concurrent execution of both threads
				pthread_create(&C3_monitor_thread , NULL, C3_monitor_function,NULL);
    			pthread_create(&C3_execution_thread , NULL, C3_execution_function,NULL);

                pthread_join(C3_execution_thread , &status);
                int sum2 = (int)status;
    		    pthread_join(C3_monitor_thread, NULL);

                close(p3[0]);
                write(p3[1],&sum2,sizeof(sum2));
                close(p3[1]);
                }
            }
            else{

                
 
                int c1_sum,c3_sum;

                strcpy(MC1_memory,"Start");

                if(strcmp(C1_memory,"Die")==0){
                        // Getting message via pipe from C1.
                        printf("C1_memory inside parent: %s\n",C1_memory);
                        read(p1[0],&c1_sum,sizeof(c1_sum));
                        close(p1[0]);
                        printf("C1 output: %d\n",c1_sum);

                        strcpy(MC2_memory,"Start");

                        if(strcmp(C2_memory,"Die")==0){

                            strcpy(MC3_memory,"Start");

                            if(strcmp(C3_memory,"Die")==0){
                                    // Getting message via pipe from C3.
                                    read(p3[0],&c3_sum,sizeof(c3_sum));
                                    close(p3[0]); 
                                    printf("C3 output: %d\n",c3_sum);
                                
                            }
                        }
                }
                
                

                
               

               
            }
        }
        }
    }