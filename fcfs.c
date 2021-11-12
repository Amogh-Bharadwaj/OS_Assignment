// CPP code to create three child
// process of a parent
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

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;

char* C1_memory;
char* C2_memory;
char* C3_memory;
char* results1;
char* results3;

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
 
    FILE* fp;
    fp = fopen("n3.txt" , "r");
    char str[8];
	int sum=0;
 
    

    do {
        printf("[C3]: Locked by monitor...\n");
		//pthread_mutex_lock(&mutex3);

        sum += atoi(str);
        //pthread_mutex_unlock(&mutex3);
    }while(fgets(str,10,fp)!=NULL);


  C3_memory="Die,C3";
  pthread_exit(NULL);
 
}
 
void* C3_monitor_function(void *arg){
	while(C3_memory!="Die,C3"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C3 MONITOR THREAD]: Locking execution thread.\n");
        
        if(C3_memory=="Lock C3")pthread_mutex_lock(&mutex3);
        
        sleep(3);
        
        printf("[C3 MONITOR THREAD]: Unlocking execution thread.\n");
        
        if(C3_memory=="Wake C3")pthread_mutex_unlock(&mutex3);
        sleep(1);

    }
    printf("[C3 MONITOR THREAD]: I'm done.");
	
}
 
void* C2_execution_function(void *arg)
{
    FILE* fp1;
    fp1 = fopen("n2.txt","r");
    char str[8];
   
    
	do{

		//pthread_mutex_lock(&mutex2);
        int num = atoi(str);
        printf("[C2]:Number: %d\n" , num);
        //pthread_mutex_unlock(&mutex2);
      } while(fgets(str,10,fp1)!=NULL);
	
	C2_memory="Die,C2";
 
  pthread_exit(NULL);
 
}
 
void* C2_monitor_function(void *arg){
	while(C2_memory!="Die,C2"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        printf("[C2 MONITOR THREAD]: Locking execution thread.\n");
        
        if(C2_memory=="Lock C2")pthread_mutex_lock(&mutex2);
        
        
        printf("[C2 MONITOR THREAD]: Unlocking execution thread.\n");
        
        if(C2_memory=="Wake C2")pthread_mutex_unlock(&mutex2);
        sleep(1);

    }
    printf("[C2 MONITOR THREAD]: I'm done.");
	
}
 

void* C1_execution_function(void* argument){
    
    int arg = VOID_TO_INT(argument);
    printf("Enter number of values when execution thread isn't sleeping:\n");
    int n;
    scanf("%d",&n);

    for(int i=0;i<n;i++){ 
        //pthread_mutex_lock(&mutex1);
        
        //printf("[C1]: Unlocked by monitor...\n");
        
        //Critical section
        int x;
        scanf("[C1]: Enter a number: %d\n",&x);
        arg += x;  
        //printf("[C1]: Executing.\n");
        //pthread_mutex_unlock(&mutex1);
           
    }
    printf("[C1]: SUM: %d\n",arg);
    C1_memory="Die,C1";
}

void* C1_monitor_function(){
    
    while(C1_memory!="Die,C1"){
        //Just for smooth running, we put to sleep for a few seconds.
        sleep(1);
        //printf("[C1 MONITOR THREAD]: Locking execution thread.\n");
        
        if(C1_memory=="Lock C1")pthread_mutex_lock(&mutex1);
        sleep(3);
        
        printf("[C1 MONITOR THREAD]: Unlocking execution thread.\n");
        
        if(C1_memory=="Wake C1")pthread_mutex_unlock(&mutex1);
        sleep(1);
    }
    printf("[C1 MONITOR THREAD] I'm done.");
}
 

// Driver code
int main()
{
	int pid, pid1, pid2;
    int p1[2],p3[2];
    
    //creating pipes
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

   
    // List of all shared memories.
    int shmid1 = shmget(ftok("/etc",65),1024,0666|IPC_CREAT);
    C1_memory=(char*) shmat(shmid1,(void*)0,0);

    int shmid2 = shmget(ftok("/var",65),1024,0666|IPC_CREAT);
    C2_memory=(char*) shmat(shmid2,(void*)0,0);

    int shmid3 = shmget(ftok("/dev",65),1024,0666|IPC_CREAT);
    C3_memory=(char*) shmat(shmid3,(void*)0,0);

    int shmid4 = shmget(ftok("/tmp",65),1024,0666|IPC_CREAT);
    results1=(char*) shmat(shmid4,(void*)0,0);

    int shmid5 = shmget(ftok("/bin",65),1024,0666|IPC_CREAT);
    results3=(char*) shmat(shmid5,(void*)0,0);


 
	// variable pid will store the
	// value returned from fork() system call
	pid = fork();
 
	// If fork() returns zero then it
	// means it is child process.
	if (pid == 0) {

        C2_memory="Lock C2";
        C3_memory="Lock C3";
        C1_memory="Wake C1";

		pthread_t C1_monitor_thread;
		pthread_t C1_execution_thread;
    
    	long sum = 0;

		//Concurrent execution of both threads
		pthread_create(&C1_monitor_thread , NULL, C1_monitor_function,NULL);
    	pthread_create(&C1_execution_thread , NULL, C1_execution_function,INT_TO_VOID(sum));

		//pthread_join waits for the threads passed as argument to finish(terminate).
    	pthread_join(C1_execution_thread , NULL);
    	pthread_join(C1_monitor_thread, NULL);
    	
        sum = sti2(results1);
		printf("Sum: %ld\n",sum);

        close(p1[0]);
        write(p1[1],&sum,sizeof(sum));
        close(p1[1]);

		
	}

	else {
        //wait(NULL);
		pid1 = fork();
		if (pid1 == 0) {
            
            if(C1_memory=="Die,C1"){

            C2_memory="Wake C2";
            C3_memory="Lock C3";
            C1_memory="Lock C1";
            

			pthread_t C2_monitor_thread;
			pthread_t C2_execution_thread;
    
			//Concurrent execution of both threads
			pthread_create(&C2_monitor_thread , NULL, C2_monitor_function,NULL);
    		pthread_create(&C2_execution_thread , NULL, C2_execution_function,NULL);

			//pthread_join waits for the threads passed as argument to finish(terminate).
    		pthread_join(C2_execution_thread , NULL);
    		pthread_join(C2_monitor_thread, NULL);
            }
		}
		else {
            //wait(NULL);
			pid2 = fork();
			if (pid2 == 0) {
                if(C1_memory=="Die,C1" && C2_memory=="Die,C2"){
                C2_memory="Lock C2";
                C3_memory="Wake C3";
                C1_memory="Lock C1";
               
                
				pthread_t C3_monitor_thread;
				pthread_t C3_execution_thread;
                long sum2 = 0;
				
				//Concurrent execution of both threads
				pthread_create(&C3_monitor_thread , NULL, C3_monitor_function,NULL);
    			pthread_create(&C3_execution_thread , NULL, C3_execution_function,INT_TO_VOID(sum2));

				//pthread_join waits for the threads passed as argument to finish(terminate).
    			pthread_join(C3_execution_thread , NULL);
    			pthread_join(C3_monitor_thread, NULL);

                sum2 = sti2(results3);
 
				//sending output via pipes
                close(p3[0]);
                write(p3[1],&sum2,sizeof(sum2));
                close(p3[1]);
                }
		
			}
 
			// If value returned from fork()
			// in not zero and >0 that means
			// this is parent process.
			else {
				
				//wait(NULL);
 
                int c1_sum,c3_sum;
 
                //getting message via pipe from C1
                read(p1[0],&c1_sum,sizeof(c1_sum));
                close(p1[0]);
                printf("C1 output: %d\n",c1_sum);

                //getting message via pipe from C3
                read(p3[0],&c3_sum,sizeof(c3_sum));
                close(p3[0]);
                printf("C3 output: %d\n",c3_sum);
                
				printf("parent --> pid = %d\n", getpid());
			}
		}
	}
 
	return 0;
}