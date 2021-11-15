#include<stdio.h>
#include<string.h>
#include <unistd.h>



int main(){
    char s[5];
    printf("Enter choice of scheduling algorithm - (fcfs (or) rr): ");
    scanf("%s",s);
    
    // Running the required executable as per user input.
    if(strcmp(s,"fcfs")==0) execl("./fcfs","FCFS",NULL);
    else if(strcmp(s,"rr")==0) execl("./rr","RR",NULL);
    else printf("Invalid option.\n");
    return 0;
    
}