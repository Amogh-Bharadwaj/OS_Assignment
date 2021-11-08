#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include<semaphore.h>
 
 
 
sem_t sema1;
sem_t sema2;
sem_t sema3;
 
long sum = 0;
 
void* c3(void *arg)
{
 
    FILE* fp;
    fp = fopen("n3.txt" , "r");
    char str[8];
 
 
 
    do {
        sem_wait(&sema3);
        sum += atoi(str);
        sem_post(&sema3);
 
 
    }while(fgets(str,10,fp)!=NULL)
 
  pthread_exit(NULL);
 
}
 
void* c3c(void *arg){
    
}
 
void* c2(void *arg)
{
    FILE* fp1;
    fp1 = fopen("n2.txt","r");
    char str[8];
 
    do{
        sem_wait(&sema2);
        int num = atoi(str);
        printf("%d\n" , num);
        sem_post(&sema2);
 
    }while(fgets(str,10,fp1)!=NULL)
 
  pthread_exit(NULL);
 
}
 
void* c2c(void *arg){
    
}
 
 
void* c1(void *arg)
{
    int n;
    scanf("%d" , &n);
    long sum1 = 0;
    for(int i = 0 ;i<n;i++)
    {
        sem_wait(&sema1);;
        int x;
        scanf("%d",&x);
        sum1 += x;
        sem_post(&sema1);
 
    }
 
    pthread_exit(NULL);
 
}
 
void* c1c(void *arg){
    
}
 
int main(int argc , char *argv [])
{
    p_thread_t thread_c1;
    p_thread_t thread_c2;
    p_thread_t thread_c3;
    sem_init(&sema1, 0 , 0);
    sem_init(&sema2, 0 , 0);
    sem_init(&sema3, 0 , 0);
    
    pthread_create(&thread_c1 , NULL, c1,NULL);
 
    pthread_join(thread_c1 , NULL);
    
    
    pthread_create(&thread_c2 , NULL, c2,NULL);
 
    pthread_join(thread_c2 , NULL);
    
    pthread_create(&thread_c3 , NULL, c3,NULL);
 
    pthread_join(thread_c3 , NULL);
 
    sem_destroy(&sema1);
    sem_destroy(&sema2);
    sem_destroy(&sema3);
   
 
 
}
