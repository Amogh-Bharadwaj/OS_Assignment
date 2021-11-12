
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

char* C1_memory; // Interthread communication (monitor thread of C1 to execution thread of C1).
char* C2_memory;
char* C3_memory;
char* MC1_memory; // This is for Main to the three processes.
char* MC2_memory;
char* MC3_memory;


// Union and two functions to convert void* to int. 
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

// Same as above function but takes char pointer as argument.
int sti2(char* s){
    int num = 0;
    int l = strlen(s);
   
    for(int i=0;i<l-1;i++){
        num+=(int)(s[i]-'0');
        num*=10;
    }
    
    num=num/10;
    return num;
}

void* C3_execution_function(void *arg)
{
    
    // Opening n3 text file.
    FILE* fp;
    fp = fopen("n3.txt" , "r");

    char str[8];
	int sum=0;

    do {

        int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
        C3_memory=(char*) shmat(shmid3,(void*)0,0);
        
        // C3 is asleep until monitor tells to wake up.
		while(C3_memory!="Start"){
            printf("[C3]: Locked by monitor...\n");
            //printf("Shared memory inside C3: %s\n",shared_memory);
            sleep(1);
        }

        sum += atoi(str);
    }while(fgets(str,10,fp)!=NULL);

    
    int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
    C3_memory=(char*) shmat(shmid3,(void*)0,0);
    
    // Intimating to C3-monitor thread that C3 is over.
    C3_memory="Die";

    return (void *)sum;

}

void* C3_monitor_function(void *arg){
    int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
    C3_memory=(char*) shmat(shmid3,(void*)0,0);

    // Monitor should die when C3 is over.
	while(C3_memory!="Die"){
        
        printf("[C3 MONITOR THREAD]: Locking execution thread.\n");

       int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
       C3_memory=(char*) shmat(shmid3,(void*)0,0);
       
       // C3 Monitor will die when C3 dies.
       if(C3_memory!="Die"){
          

            int shmid6 = shmget(ftok("/var",65),1024,0666|IPC_CREAT);
            MC3_memory=(char*) shmat(shmid6,(void*)0,0);

            // When scheduler tells to stop.
            if(MC3_memory=="C3 Stop"){
                
               int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
               C3_memory=(char*) shmat(shmid3,(void*)0,0);
                
                // Monitor tells execution thread to stop.
                C3_memory="Stop";
            }
        }
        else break;


        printf("[C3 MONITOR THREAD]: Unlocking execution thread.\n");

       int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
       C3_memory=(char*) shmat(shmid3,(void*)0,0);
       
       // C3 Monitor dies when C3 dies.
       if(C3_memory!="Die"){
          

            int shmid6 = shmget(ftok("/var",65),1024,0666|IPC_CREAT);
            MC3_memory=(char*) shmat(shmid6,(void*)0,0);
 
            // If scheduler tells to start
            if(MC3_memory=="C3 Start"){
                
               int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
               C3_memory=(char*) shmat(shmid3,(void*)0,0);
                
                // Then C3 will start.
                C3_memory="Start";
            }
        }
        else break;

	
    }
    printf("[C3 MONITOR THREAD]: I'm done.");

}

void* C2_execution_function(void *arg)
{   
    // Opening n2 file.
    FILE* fp1;
    fp1 = fopen("n2.txt","r");
    char str[8];

    
	do{
        int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
        C2_memory=(char*) shmat(shmid2,(void*)0,0);

		while(C2_memory!="Start"){
            printf("[C2]: Locked by monitor...\n");
            //printf("Shared memory inside C2: %s\n",shared_memory);
            sleep(1);
        }

        int num = atoi(str);
        printf("%d\n" , num);
      } while(fgets(str,10,fp1)!=NULL);


    int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
    C2_memory=(char*) shmat(shmid2,(void*)0,0);

    // Intimating to monitor that C2 is over.
	C2_memory="Die";

  
}

void* C2_monitor_function(void *arg){

    int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
    C2_memory=(char*) shmat(shmid2,(void*)0,0);

	while(C2_memory!="Die"){
    
        printf("[C2 MONITOR THREAD]: Locking execution thread.\n");

        int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
        C2_memory=(char*) shmat(shmid2,(void*)0,0);
       
       // Monitor dies when C2 dies.
       if(C2_memory!="Die"){
            int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
            C2_memory=(char*) shmat(shmid2,(void*)0,0);


            int shmid5 = shmget(ftok("/dev",65),1024,0666|IPC_CREAT);
            MC2_memory=(char*) shmat(shmid5,(void*)0,0);
            
            // If scheduler tells to stop
            if(MC2_memory=="C2 Stop"){
                
                int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
                C2_memory=(char*) shmat(shmid2,(void*)0,0);
                
                // Monitor tells execution to stop.
                C2_memory="Stop";
            }
        }
        else break;


        printf("[C2 MONITOR THREAD]: Unlocking execution thread.\n");

        int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
        C2_memory=(char*) shmat(shmid2,(void*)0,0);

        
        // Monitor dies when C2 dies.
        if(C2_memory!="Die"){
            int shmid5 = shmget(ftok("/dev",65),1024,0666|IPC_CREAT);
            MC2_memory=(char*) shmat(shmid5,(void*)0,0);
            
            // If scheduler says to start.
            if(MC2_memory=="C2 Start"){

                 int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
                 C2_memory=(char*) shmat(shmid2,(void*)0,0);
                // Monitor will tell execution thread to start.
                C2_memory="Start";
            }
            }
        else break;

		printf("[C2 MONITOR THREAD]: C2 memory: %s",C2_memory);
    }
    printf("[C2 MONITOR THREAD]: I'm done.");

}


void* C1_execution_function(void* argument){

    int arg = VOID_TO_INT(argument);
    printf("Enter number of values when execution thread isn't sleeping:\n");
    int n;
    scanf("%d",&n);

    for(int i=0;i<n;i++){ 

        int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        C1_memory=(char*) shmat(shmid1,(void*)0,0);
        
        // Unless monitor tells me to start, I will be asleep.
        while(C1_memory!="Start"){
            printf("[C1]: Locked by monitor...\n");
            //printf("[C1]: Shared memory inside the while loop: %s\n",shared_memory);

            sleep(1);
        }

        printf("C1 memory after the while loop: %s\n",C1_memory);

        printf("[C1]: Unlocked by monitor...\n");

        //Critical section
        int x;
        printf("[C1]: Enter a number: ");
        scanf("%d",&x);
        arg += x;  
        

    }
    printf("[C1]: SUM: %d\n",arg);
    
    int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
    C1_memory=(char*) shmat(shmid1,(void*)0,0);

    // Intimating to monitor that execution is over.
    C1_memory="Die";
    return (void *)arg;
}

void* C1_monitor_function(){

    int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
    C1_memory=(char*) shmat(shmid1,(void*)0,0);

    while(C1_memory!="Die"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C1 MONITOR THREAD]: Locking execution thread.\n");

        int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        C1_memory=(char*) shmat(shmid1,(void*)0,0);
        
        // If C1 is over, monitor thread should terminate.
        if(C1_memory!="Die"){
        
            int shmid4 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
            MC1_memory=(char*) shmat(shmid4,(void*)0,0);

            if(MC1_memory=="C1 Stop"){

                int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
                C1_memory=(char*) shmat(shmid1,(void*)0,0);

                C1_memory="Stop";
            }
        }
        else break;
       
       int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
       C1_memory=(char*) shmat(shmid1,(void*)0,0);
        printf("[C1 MONITOR THREAD]: Shared memory: %s",C1_memory);


        printf("[C1 MONITOR THREAD]: Unlocking execution thread.\n");

       


        int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
        C1_memory=(char*) shmat(shmid1,(void*)0,0);
        
        // If C1 is over, then we terminate this monitor thread.
        if(C1_memory!="Die"){

            int shmid4 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
            MC1_memory=(char*) shmat(shmid4,(void*)0,0);


           
            // If scheduler says to start.
            if(MC1_memory=="C1 Start"){
                int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
                C1_memory=(char*) shmat(shmid1,(void*)0,0);
                
                // Monitor tells execution thread to start.
                C1_memory="Start";
            }
        }
        else break;

        

    }
    printf("[C1 MONITOR THREAD] I'm done.");
}

int main()
{   //Process PIDs.
	int pid, pid1, pid2;

    // Pipes.
    int p1[2],p3[2];

    // List of shared memories. 
    // MC1,MC2,MC3 are main-to-process communication
    // C1,C2,C3 are for monitor-to-execution thread communication.
    int shmid1 = shmget(ftok("./",65),1024,0666|IPC_CREAT);
    C1_memory=(char*) shmat(shmid1,(void*)0,0);

    int shmid4 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
    MC1_memory=(char*) shmat(shmid4,(void*)0,0);

    int shmid2 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
    C2_memory=(char*) shmat(shmid2,(void*)0,0);

    int shmid5 = shmget(ftok("/dev",65),1024,0666|IPC_CREAT);
    MC2_memory=(char*) shmat(shmid5,(void*)0,0);

    int shmid3 = shmget(ftok("/opt",65),1024,0666|IPC_CREAT);
    C3_memory=(char*) shmat(shmid3,(void*)0,0);

    int shmid6 = shmget(ftok("/var",65),1024,0666|IPC_CREAT);
    MC3_memory=(char*) shmat(shmid6,(void*)0,0);

    
    
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

        int shmid4 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
        MC1_memory=(char*) shmat(shmid4,(void*)0,0);
        MC1_memory="C1 Start"; // Setting to start so C1 will start immediately.

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

        printf("Sum inside C1 child: %ld\n",sum);

        close(p1[0]);
        write(p1[1],&sum,sizeof(sum)); // Writing sum to pipe.
        close(p1[1]);

    }

        else {
        //wait(NULL);
		pid1 = fork();
		if (pid1 == 0) {
            //C2

            int shmid5 = shmget(ftok("/dev",65),1024,0666|IPC_CREAT);
            MC2_memory=(char*) shmat(shmid5,(void*)0,0);
            MC2_memory="C2 Start";

            pthread_t C2_monitor_thread;
			pthread_t C2_execution_thread;

			//Concurrent execution of both threads
			pthread_create(&C2_monitor_thread , NULL, C2_monitor_function,NULL);
    		pthread_create(&C2_execution_thread , NULL, C2_execution_function,NULL);

            //pthread_join waits for the threads passed as argument to finish(terminate).
    		pthread_join(C2_execution_thread , NULL);
    		pthread_join(C2_monitor_thread, NULL);
		}
		else {
            //wait(NULL);
			pid2 = fork();
			if (pid2 == 0) {

                //C3

                int shmid6 = shmget(ftok("/var",65),1024,0666|IPC_CREAT);
                MC3_memory=(char*) shmat(shmid6,(void*)0,0);
                MC3_memory="C3 Start";

				pthread_t C3_monitor_thread;
				pthread_t C3_execution_thread;
                

                void* status;

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
            else{

                //wait(NULL);
 
                int c1_sum,c3_sum;
                
                // Getting message via pipe from C1.
                read(p1[0],&c1_sum,sizeof(c1_sum));
                close(p1[0]);
                printf("C1 output: %d\n",c1_sum);

                //getting message via pipe from C3.
                read(p3[0],&c3_sum,sizeof(c3_sum));
                close(p3[0]);

                
            }
        }
        }
    }

    