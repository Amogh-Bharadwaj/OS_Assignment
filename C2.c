#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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


int main(){

    FILE *fp;
    fp = fopen("n2.txt","r");
    
    //Assuming a max of 8 digits per number
    char str[8];
    long sum = 0;
    
    //Reading and printing numbers
    while(fgets(str,10,fp)!=NULL)printf("%d\n",sti(str));

    return 0;
}