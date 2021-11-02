// CPP code to create three child
// process of a parent
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
 
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
 
 
	// variable pid will store the
	// value returned from fork() system call
	pid = fork();
 
	// If fork() returns zero then it
	// means it is child process.
	if (pid == 0) {
        //C1
        

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
	
    	

        //sending output via pipes
        close(p1[0]);
        write(p1[1],&sum,sizeof(sum));
        close(p1[1]);
 
		printf("child[1] --> pid = %d and ppid = %d\n",
			getpid(), getppid());
        //execlp("./C1.out", "C1", "3", NULL);
	}

	else {
        wait(NULL);
		pid1 = fork();
		if (pid1 == 0) {
            //C2
			FILE *fp;
    		fp = fopen("n2.txt","r");
		
    		//Assuming a max of 8 digits per number
    		char str[8];
    		long sum = 0;
		
    		//Reading and printing numbers
    		while(fgets(str,10,fp)!=NULL)printf("%d\n",sti(str));

			printf("child[2] --> pid = %d and ppid = %d\n",
				getpid(), getppid());
            //execlp("./C2.out", "C2", NULL);
		}
		else {
            wait(NULL);
			pid2 = fork();
			if (pid2 == 0) {
                
                //C3
				
				FILE *fp;
    			fp = fopen("n3.txt","r");

    			//Assuming a max of 8 digits per number. 
    			char str[8];

    			long sum = 0;

    			//Reading numbers from file and adding 
    			while(fgets(str,10,fp)!=NULL)sum+=sti(str);

    			
				//sending output via pipes
                close(p3[0]);
                write(p3[1],&sum,sizeof(sum));
                close(p3[1]);
				printf("child[3] --> pid = %d and ppid = %d\n",
					getpid(), getppid());
                //execlp("./C3.out", "C3", NULL);
			}
 
			// If value returned from fork()
			// in not zero and >0 that means
			// this is parent process.
			else {
				
				wait(NULL);
 
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