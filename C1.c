#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(){
    int n;
    printf("Enter number of numbers to be added: ");
    scanf("%d", &n);
    int sum=0;
    
    
    printf("Enter %d numbers below:\n", n);
    for(int i=0;i<n;i++){
        int x;
        scanf("%d",&x);
        sum+=x;
    }
    
    printf("Sum: %d\n",sum);
    
    return 0;
}