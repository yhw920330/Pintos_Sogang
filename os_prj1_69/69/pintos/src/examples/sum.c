#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"
#include "../lib/user/syscall.h"

int main(int argc, char *argv[])
{
	int res_fib=0;
	int res_sum=0;
	int arg1,arg2,arg3,arg4;
//	sscanf(argv[1],"%d",&arg1);
	arg1= atoi(argv[1]);
//	sscanf(argv[2],"%d",&arg2);
//	sscanf(argv[3],"%d",&arg3);
//	sscanf(argv[4],"%d",&arg4);
	arg2= atoi(argv[2]);
	arg3= atoi (argv[3]);
	arg4= atoi(argv[4]);
	res_fib=fibonacci(arg1);
	res_sum=sum_of_four_integers(arg1,arg2,arg3,arg4);
	printf("%d %d\n",res_fib,res_sum);

}
