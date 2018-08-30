#include <stdio.h>
#include "myprintf.h"

/**************************** PART 1 ***********************************/
int getebp(void);
int A(int, int);
int B(int, int);
int C(int, int);

int *FP;

int main(int argc, char *argv[], char *env[])
{
	int a,b,c;
	printf("enter main\n");
  
	printf("&argc=%p argv=%p env=%p\n", (void*)&argc, (void*)argv, (void*)env);
	printf("&a=%p &b=%p &c=%p\n", (void*)&a, (void*)&b, (void*)&c);

	//(1). Write C code to print values of argc and argv[] entries
	myprintf("argc=%d\n", argc);
	for(int i = 0; i < argc; i++)
		myprintf("argv[%d]=%s\n",i,argv[i]);

	//part 3. use myprintf to print values of env
	for(int i = 0; env[i] != 0; i++)
		myprintf("env[%d]=%s\n", i, env[i]);

	a=1; b=2; c=3;
	A(a,b);
	printf("exit main\n");
}

int A(int x, int y)
{
    int d,e,f;
    printf("enter A\n");
    // PRINT ADDRESS OF d, e, f
	printf("&d=%p &e=%p &f=%p\n",(void*)&d,(void*)&e,(void*)&f);
    d=4; e=5; f=6;
    B(d,e);
    printf("exit A\n");
}

int B(int x, int y)
{
    int g,h,i;
    printf("enter B\n");
    // PRINT ADDRESS OF g,h,i
	printf("&g=%p &h=%p &i=%p\n",(void*)&g, (void*)&h, (void*)&i);
    g=7; h=8; i=9;
    C(g,h);
    printf("exit B\n");
}

int C(int x, int y)
{
    int u, v, w, i, *p;

    printf("enter C\n");
    // PRINT ADDRESS OF u,v,w,i,p;
	printf("&u=%p &v=%p &w=%p &i=%p &p=%p\n",(void*)&u,(void*)&v,(void*)&w,(void*)&i,(void*)&p);
    u=10; v=11; w=12; i=13;

    FP = (int *)getebp();
	
    //(2). Write C code to print the stack frame link list
	printf("\nSTACK FRAME LINKED LIST\n");
	while(FP != 0)
	{
		printf("%p ->", (void*)FP);
		FP = (int *)*FP;
	}
	printf("0\n");

    p = (int *)&p;

    //(3). Print the stack contents from p to the frame of main()
    //YOU MAY JUST PRINT 128 entries of the stack contents.
	FP = (int *)getebp();
	while(*FP != 0)
		FP = (int *)*FP;
	printf("\nSTACK CONTENTS\n");
	for(int * i = p; i <= FP; i++)
		printf("%p: %x\n", (void*)i, *i);
		

    //(4). On a hard copy of the print out, identify the stack contents
    //as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
}


