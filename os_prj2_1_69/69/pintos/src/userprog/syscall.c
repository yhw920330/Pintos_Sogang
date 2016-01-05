#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"//shutdown_power_off
#include "process.h"//process_execute ,process_wait
#include "threads/vaddr.h" //phys_base 
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

	void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


//intr_frame : interrupt frame structure 
	static void
syscall_handler (struct intr_frame *f UNUSED) 
{

//	hex_dump(f->esp,f->esp, 0x90,true);
	//////////////////////////
//	printf("@!@#!#!@#!");
	int syscall_num;
	syscall_num = *(int *) f->esp;
	if (!is_user_vaddr(f->esp)) {
		exit(-1);
	}
	int *tmp=(int*)(f->esp);


	if(syscall_num == SYS_HALT){
		halt();

	}
	else if(syscall_num == SYS_EXIT){

		int ch= check_addr((const void*)&tmp[1]);
		if(ch==1)
		{
			exit(tmp[1]);
		}
		else{
			//	printf("혹시여기?");
				exit(-1);//
		}
	
	}

	else if(syscall_num == SYS_EXEC){

		int tp = check_addr(( const void *)tmp[1]);
		if (tp==0)
		{
		//	printf("hadler: exys_exec\n");	
			exit(-1);
		}
		else
		{
			f->eax= exec( (char *)tmp[1]);
		}	
		
	}
	else if(syscall_num == SYS_WAIT){
	//	printf("syscall_wait시작\n");
	//여기서 page_Fault생김 	
		pid_t arg=(pid_t)tmp[1];
	//	printf("syscall_wait 중간\n");
		f->eax=wait(arg);
	//	printf("syscall_wait끝\n");
	}


	else if(syscall_num == SYS_READ){

		int fd=tmp[5];
		void*  buff= (void *) tmp[6];
		uint32_t size= (uint32_t) tmp[7];

		f->eax= read(fd, buff, size);


	}


	else if (syscall_num == SYS_WRITE){
		int fd=tmp[5];
		void* buff=  (void*)tmp[6];
		uint32_t size = tmp[7];

		f->eax=write(fd, buff, size);

	}

	else if(syscall_num == SYS_FIBO){
		int arg=tmp[1];
		
		f->eax=fibonacci(arg);
	}

	else if(syscall_num ==SYS_SUMof4INT){
		int arg1=tmp[6]; //왜이런지 모르겠음 그냥 그렇게 저장되어있음
		int arg2=tmp[7]; //
		int arg3=tmp[8];
		int arg4=tmp[9];
		f->eax= sum_of_four_integers(arg1,arg2,arg3,arg4);
	}
	else{
	//extra Syscall 	
	}

}
///prj2_10_28////////lib/syscall-nr.h에////////////////////////////////
/*
   1) interupt 의 esp를 보고 syscall_num 저장
   2) syscall에 맞게 구현 -> 핀토스 매뉴얼에서 사용하라는 함수 사용
   3) syscall_handler은 syscall_num 에따라서 함수 호출 하게 만들고
   4) 각함수 halt exit exe wait 함수들을 구현
   5) 추가적 fib/sum구현하고 
   6) fib/sum에 해당하는 syscall_num을 부여 
 */

void halt(void)
{
	shutdown_power_off();
}
void exit(int status)
{ 
	//prj2_1 2015.10.30 14:12/////////////////////
	//  1. 부모의 block을 unblock으로
	//  2.  exit_Status, exit_accepted,die  조정 
	//  3.  나의 child들을 exit_status를 -1로 하고 
	//  4.  thread_exit()
	////////////////////////////////
	struct thread *my =thread_current();

	struct thread *parent =my->parent;
	struct list* my_child= &(my-> child);
	
	my->exit_status = status;
    my->exit_accepted =0 ;

	//부모가 wait하는 상태일때
	if( parent ->status ==THREAD_BLOCKED )
	{
		thread_unblock(parent);// 부모를 wait에서 깨움
		parent->status =THREAD_RUNNING;
	
	}
	printf("%s: exit(%d)\n",my->name,my->exit_status);

	while(my->exit_accepted ==0); 
	thread_exit();
}

pid_t exec (const char *cmd_line)
{
//	printf("exec!!n");
	return process_execute(cmd_line);


}


int wait(pid_t pid)
{
	//process wait (현재 무한 루프 돌게 해놓음(2015/10/28) 고칠것
	return process_wait(pid);

}

int read(int fd,void *buffer, unsigned size){
	//input_getc()함수 이용하래 ->getchar 함수 같음
	//prototype : uint8_t input_getc (void) 
	unsigned int i;
	if(fd==0){ //fd가 0 일때는 keyboadrd에서 읽는것

		for(i=0; i< size; i++)
			*(uint8_t *) (buffer+i) = input_getc();

		if(i==size) //끝까지 읽었을때
			return size;
		else 
			return -1;
	}
	else{//fd로 file을 찾아서 읽게 해야함 (prj2_1범위 밖)

	//	printf("Not yet(STD IN 아닐때!\n");
		return -1;
	}


}


int write( int fd, const void *buffer, unsigned size)
{

	//unsigned int i=0;
	if(fd==1) //stdout으로 출력하는 기능
	{

		//putbuf 이용
		///* Writes the N characters in BUFFER to the console. */
		//prototype:voidputbuf (const char *buffer, size_t n) 
		//size가 크면 나눠서 해줘야하나? 
		putbuf(buffer,size);	
		return size;	
	}
	else
	{
		//fd로 file찾아서 쓰는 것
	//	printf("wrtie_fd>0( STDOUT 아닐때 !\n");
		//putbuf(buffer,size);	
		return -1;
	}
}

int fibonacci(int n)
{
	int x0=1,x1=1;
	int i;
	int tmp;
	if(n==1 || n==2)
		return 1;
	else
	{
		//x0 << x1
		//      x0 <<x1
		for(i=3; i<=n; i++)
		{
			tmp=x0+x1;
			x0=x1;
			x1=tmp;
		}
	}

	return x1;


}



int sum_of_four_integers(int a, int b, int c, int d)
{
	return a+b+c+d;
}


int check_addr(const void *addr)
{
	if(addr >= PHYS_BASE)
		return 0;
//	if(addr == NULL)
//		return 0;
	//unmapped에 대한 건 어떻게 하지?
	return 1;

}


//(todo 추가구현)-------------read/write fd로 file쓰기 하기 위한 file_search

