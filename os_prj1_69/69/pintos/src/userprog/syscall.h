#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include<stdio.h>
typedef int pid_t;
//#define PID_ERROR ((pid_t) -1)           //update : error value for pid_t 2015.10.30


void syscall_init (void);
//static void syscall_handler(struct intr_frame *);

int check_addr (const void *addr);
void halt(void);
void exit(int status);
pid_t exec (const char *);
int wait(pid_t pid);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
int fibonacci(int n);
int sum_of_four_integers(int a, int b, int c, int d);


////prj2_2
bool create (const char *file, unsigned initial_size);
bool remove( const char *file);
int open (const char *file);
void close( int fd);
int filesize (int fd);
void seek(int fd, unsigned position);
unsigned tell( int fd);

#endif /* userprog/syscall.h */
