#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <stdbool.h>

#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h" //phys_base 
#include "threads/synch.h"
#include "threads/malloc.h"
#include "threads/palloc.h"


#include "devices/input.h"
#include "devices/shutdown.h"//shutdown_power_off
#include "process.h"//process_execute ,process_wait

#include "../filesys/file.h"
#include "../filesys/filesys.h"
#include "../filesys/inode.h"


////////////////prj2_2 11.07
struct semaphore mutex;
struct semaphore wrt;
int readcount=0;
static struct lock open_lock;
int sys_fd=1;
//////////////

struct n_file * get_fd ( int fd);
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
	//&&& prj2_2 ///
		sema_init( &mutex,1);
		sema_init( &wrt, 1);
		readcount=0;
		lock_init(&open_lock);
		sys_fd=1;
	/////
		intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

//	hex_dump(f->esp,f->esp, 0x90,true);


	//check whether f->esp is User vaddr
	if (!is_user_vaddr(f->esp)) {
		exit(-1);
	}

	int syscall_num;
	syscall_num = *(int *) f->esp;	
	int *tmp=(int*)(f->esp);

	//case 1 ) SYS_HALT
	if(syscall_num == SYS_HALT){
		halt();
	}

	//case 2) sys_EXIT
	else if(syscall_num == SYS_EXIT){
		if(is_user_vaddr( (const void *) &tmp[1]))
			exit(tmp[1]);
		else
			exit(-1);
	}

	//case 3) SYS_EXEC
	else if(syscall_num == SYS_EXEC){
		if(is_user_vaddr ( (const void *) tmp[1]))
		{
			f->eax = exec( ( const void *) tmp[1]);
		}
		else
			exit(-1);
	}

	//case 3) sys_wait
	else if(syscall_num == SYS_WAIT){
		pid_t arg=(pid_t)tmp[1];
		f->eax=wait(arg);
	}

	//case 4) SYS_READ
	else if(syscall_num == SYS_READ){

		int fd=tmp[5];
		const void*  buff= (const void *) tmp[6];
		unsigned size= (unsigned) tmp[7];
		//prj2_2 read라는 행위를 atomic operation을 만들어줌 lock을 걸어서
		lock_acquire( &my_lock);
		f->eax= read(fd, buff, size);
		lock_release( &my_lock);

	}


	//case 5) SYS_WRITE
	else if (syscall_num == SYS_WRITE){
		int fd=tmp[5];
		void* buff=  (void*)tmp[6];
		unsigned size = (unsigned )tmp[7];
		//prj2_2 write의 행위를 atomic operation으로 만들어줌  lock을 걸어서
		lock_acquire( &my_lock);
		f->eax=write(fd, buff, size);
		lock_release( &my_lock);
	}


	//case 6) SYS_FIBO
	else if(syscall_num == SYS_FIBO){
		int arg=tmp[1];
		f->eax=fibonacci(arg);
	}

	//cas 7 SYS_ SUM of 4INT
	else if(syscall_num ==SYS_SUMof4INT){
		int arg1=tmp[6]; //왜이런지 모르겠음 그냥 그렇게 저장되어있음
		int arg2=tmp[7]; //
		int arg3=tmp[8];
		int arg4=tmp[9];
		f->eax= sum_of_four_integers(arg1,arg2,arg3,arg4);
	}

	//case 8) SYS_CREATE
	else if(syscall_num == SYS_CREATE)
	{
		f->eax= create((const char*)tmp[4],(unsigned) tmp[5]);

	}
	//case 9) SYS_REMOVE
	else if(syscall_num == SYS_REMOVE)
	{
		f->eax= remove( (const char*) tmp[1]);
	}

	//case 10) SYs_ OPEN
	else if(syscall_num == SYS_OPEN)
	{
		f->eax= open ( (const char *) tmp[1]);
	}

	//case 11 SYS_CLOSE
	else if(syscall_num == SYS_CLOSE)
	{
      	close( (int) tmp[1]);
	}

	//case 12) SYs_filesize
	else if(syscall_num == SYS_FILESIZE)
	{
		f->eax= filesize( (int) tmp[1]);
	}

	//case 13) SYS_CASE SEEK
	else if( syscall_num == SYS_SEEK)
	{
		seek ( (int)tmp[4], (unsigned) tmp[5]);
	}
	//case 14) SYS_TELL
	else if( syscall_num == SYS_TELL)
	{
		f->eax= tell( (int)tmp[1]);
	}
	else{
	//extra Syscall 	
	}

}
void halt(void)
{
	shutdown_power_off();
}
void exit(int status)
{ 
	struct thread *my =thread_current();
	my->exit_status = status;
    my->exit_accepted =0 ; //나의 죽음을 확인할때 
	printf("%s: exit(%d)\n",my->name,my->exit_status);
	sema_up(&my->sema_wait); //나를 기다리고있는 부모의 wait signal을 보낸다 
	thread_exit();  // process_Exit에서 추가적으로 orphan 문제, file_list deallocate
}

pid_t exec (const char *cmd_line)
{
	return process_execute(cmd_line);
}


int wait(pid_t pid)
{
	return process_wait(pid);
}

int read(int fd,void *buffer, unsigned size)
{
	//input_getc()함수 이용하래 ->getchar 함수 같음
	//prototype : uint8_t input_getc (void) 
	unsigned int i;

	int res=0;

	//check is valid addr
	if(buffer >=PHYS_BASE  ) //@@
	{
		exit(-1);
		return -1; //@@
	}


	//case 1) STDIN 
	if(fd==0){ //fd가 0 일때는 keyboadrd에서 읽는것

		for(i=0; i< size; i++)
			*(uint8_t *) (buffer+i) = input_getc();

		if(i==size) //끝까지 읽었을때
			return size;
		else 
			return -1;
	}

	//case 2) STDOUT
	else
	{
		struct n_file *fp=NULL;

		if( fd >1){
	
			fp= get_fd( fd);

			if( fp != NULL)
			{
					sema_down (&mutex);
					readcount++;
					if(readcount ==1){
						sema_down (&wrt);
					}
					sema_up(&mutex);

					///READ is perfomed/////////////////////////////////
					res =file_read( fp->f, (void*) buffer, size);
					///////////////////////////////////////////////////
					
					sema_down(&mutex);
					readcount --;
					if( readcount ==0)
						sema_up (&wrt);
					sema_up(&mutex);

			}
		}
		else
		{
			//printf("Error userprog /syscall read함수\n");
			exit(-1);
		}
		return res;
	
	
	}
}


int write( int fd, const void *buffer, unsigned size)
{

	if(buffer >= PHYS_BASE || !is_user_vaddr (buffer + size -1) )//modified
	{
		exit(-1);
	}

	if(fd==1) //stdout으로 출력하는 기능
	{

		//prototype:voidputbuf (const char *buffer, size_t n) 
		//size가 크면 나눠서 해줘야하나? 
		putbuf(buffer,size);	
		return size;	
	}
	else
	{	
		struct n_file *fp=NULL;
		unsigned ret;

		if(fd>1) 
		{
			fp= get_fd(fd);
			if ( fp != NULL)
			{
					sema_down(&wrt);
					ret =file_write(fp->f, buffer, size);
					sema_up(&wrt);
			//		break;
			}
			return ret;
		}
		else
		{
			exit(-1);
		}

		return ret;
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
	if(addr == NULL)
		return 0;
	//unmapped에 대한 건 어떻게 하지?
	return 1;

}

//prj2_2 11.07////////////
bool create (const char *file, unsigned initial_size){
	if( file == NULL || file+ initial_size -1 >= PHYS_BASE )
	{
		exit(-1);
	}
	return filesys_create(file,initial_size);

}

bool remove( const char *file){
	if( file ==NULL  ){
		exit(-1);
	}
	return filesys_remove(file);
}

int open (const char *file){
	struct n_file *fp;
	struct thread* cur= thread_current();
	if(file==NULL )
	{
		exit(-1);
	}

//동적할당을 하여 공간을 확보 -> close에서 free시켜줘야함!!
	fp=(struct n_file*) malloc(sizeof(struct n_file));

//	fp=palloc_get_page(0);

//multioom의 해결방안!!! file.h에 정의되어잇는 file_open limt이용 
	if( list_size ( & cur ->file_list) > FILE_OPEN_LMT)
	{
		return -1;

	}

//file_sys_open을 통해 열고 
	fp->f = filesys_open(file);

//NULL이면 비정상 종료
	if(fp ->f ==NULL)
	{
		return -1;
	}

//이미 write executalbe 하면 file deny_write (fp->f) 
	if( thread_check_already_exist (file))
	{
		file_deny_write(fp->f);
	}
	
// sys_fd를 부여_ assemble어로 여러 fd가 동시에 부여받을 수 있으므로
	//이 단계를 lock을 이용하여 atomic하게 만듬
	lock_acquire (&open_lock);
	sys_fd =sys_fd+1;
	fp->fd = sys_fd;
	lock_release(&open_lock);
//현재 thread에 추가
	list_push_back ( & thread_current () -> file_list ,& fp->file_elem);

	return sys_fd ;


}
void close( int fd){

	struct list *fl = &( thread_current() -> file_list);
	struct n_file* fp=NULL;
	struct list_elem *e=NULL;
	for( e=list_begin (fl) ;
				e!= list_end(fl)
				; e=list_next(e))
	{

		fp= list_entry(e, struct n_file, file_elem);
		if(fp->fd==fd)
		{
			//정보를 삭제하고 ,thread와의 연결 끊고, free!!
			if( fp->fd == NULL) exit(-1);
			file_close(fp->f);
			list_remove(e); 
		//	palloc_free_page(fp); //!!!!!!!aDD!!
			if( fp!= NULL) free(fp); //!!free해주어서 multi-oom을 해결
			break;
		}
	}


}
int filesize (int fd){

	struct list *fl = & ( thread_current () -> file_list);

	struct n_file* fp=NULL;
	struct list_elem *e=NULL;
	for( e=list_begin (fl) ;
				e!= list_end (fl); 
				e=list_next(e))
	{

		fp= list_entry ( e, struct n_file, file_elem); 
		if(fp->fd==fd)
		{
				if(fp->fd == NULL) exit(-1);
				return file_length(fp->f);
		}
	}

	
}

void seek(int fd, unsigned position){
	struct list *fl = & ( thread_current () -> file_list);
	struct n_file* fp=NULL;
	struct list_elem *e=NULL;
	for( e=list_begin (fl) ;
				e!= list_end( fl); 
				e=list_next(e))
	{

		fp= list_entry ( e, struct n_file, file_elem); 
		if(fp->fd==fd)
		{
			if(fp->f == NULL)
						exit(-1);
			 file_seek(fp->f,position);
			break;
		}
	}


}
unsigned tell( int fd){
	struct list *fl = & ( thread_current () -> file_list);
	struct n_file* fp=NULL;
	struct list_elem *e=NULL;
	for( e=list_begin ( fl) ;
				e!= list_end( fl);
				e=list_next(e))
	{

		fp= list_entry ( e, struct n_file, file_elem); 
		if(fp->fd	==	fd)
		{
			if(fp->f == NULL)
			{
				//printf("Error user/syscall.tell이럴리 없어\n");
				exit(-1);

			}
			return file_tell(fp->f);
		}
	}
}


///////////////////
struct n_file * get_fd ( int fd)
{
	struct list_elem *e;
	struct n_file *tp=NULL;
	for ( e= list_begin ( & thread_current () -> file_list);
			e!= list_end ( & thread_current () -> file_list);
				e= list_next (e))
	{
		tp=list_entry (e, struct n_file, file_elem);
		if( tp-> fd == fd)
			return tp;
	}
	return tp;
}
