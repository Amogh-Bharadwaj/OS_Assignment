#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
    
    if(argc!=2){
        printf("Not enough arguments.");
        return 0;
    }
    
    //Pass the n1 value as command line argument (like: ./C1 9)
    int n1 = atoi(argv[1]);
    int sum=0;
    
    
    printf("Enter %d numbers below:\n", n1);
    for(int i=0;i<n1;i++){
        int x;
        scanf("%d",&x);
        sum+=x;
    }
    
    printf("Sum: %d\n",sum);
    
    return 0;
}