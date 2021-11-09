#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>
 
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
 
long sum = 0;
 
void* c3(void *arg)
{
 
    FILE* fp;
    fp = fopen("n3.txt" , "r");
    char str[8];
 
 
 
    do {
        pthread_mutex_lock(&mutex1);
        sum += atoi(str);
        pthread_mutex_unlock(&mutex1);
 
 
    }while(fgets(str,10,fp)!=NULL);
 
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
        pthread_mutex_lock(&mutex2);
        int num = atoi(str);
        printf("%d\n" , num);
        pthread_mutex_unlock(&mutex2);
 
    }while(fgets(str,10,fp1)!=NULL);
 
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
        pthread_mutex_lock(&mutex3);
        int x;
        scanf("%d",&x);
        sum1 += x;
        pthread_mutex_unlock(&mutex3);
 
    }
 
    pthread_exit(NULL);
 
}
 
void* c1c(void *arg){
    
}
 
int main(int argc , char *argv [])
{
    pthread_t thread_c1;
    pthread_t thread_c2;
    pthread_t thread_c3;
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2,NULL);
    pthread_mutex_init(&mutex3,NULL);
 
    pthread_create(&thread_c1 , NULL, c1,NULL);
 
    pthread_join(thread_c1 , NULL);
    
    
    pthread_create(&thread_c2 , NULL, c2,NULL);
 
    pthread_join(thread_c2 , NULL);
    
    pthread_create(&thread_c3 , NULL, c3,NULL);
 
    pthread_join(thread_c3 , NULL);
 
 
   
 
 
}
