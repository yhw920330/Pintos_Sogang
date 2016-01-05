#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include<stdbool.h>
#include <stdint.h>
//---------------------------------------
#include "synch.h" //prj2_2 semaphore 사용

struct lock my_lock;
extern bool thread_aging_flag;
bool thread_check_exist(const char *file);
/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */
/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */
//prj1

#define RECENT_CPU_DEFAULT 0
#define LOAD_AVG_DEFAULT 0
#define DONATE_LIMIT 8
#define NICE_MAX 20
#define NICE_MIN -20
#define NICE_DEFAULT 0

static struct list sleep_list;

////////
/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
	//int iszombie;
#endif

    /* Owned by thread.c. */
 
 ////////////////////////////////////
///prj2_1 10_30: 13:00pm
//prj2_2 11_12: modified:


	//connected to parent
    struct thread *parent; // 
    struct list_elem gene; //부모의 child_list에 추가하려고 필요함
   
	//create child
	struct list child;
    int fork_cnt; 
	bool init_childlist_flag;

	//exit_child
	int exit_status;  //
    bool exit_accepted; //초기값 (0) ,accepted =1
	
	
	tid_t waitfor; // cur : parent  -> wait for which thread
	
	struct semaphore sema_wait; // semaphore using for exit & wait
	struct semaphore sema_orphan; // semaphore usfing for preventing orphan thread

	//file_list
	struct list file_list;  // manage the file_list

    //prj1  sleep_time과 nice recent_cpu등을 담을 변수를 추가한다
	int64_t sleep_time; //sleep_time
	int nice;  //niceness
	int recent_cpu;	 //recent_cpu
  	////////////////////////////////////////
    unsigned magic;                     /* Detects stack overflow. */

  };


///////////////////////
/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);




/////prj1//////////////////////////////////////////
bool cmp_time(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

bool cmp_pri(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
void thread_sleep(int64_t waketime);
void thread_wakeup(void);
int load_avg;


//just for debugging
void print_sleep(void);
void print_all(void);
void print_ready(void);
void print_all_thread(void);

void thread_aging(void);
void increment_recent_cpu( struct thread *t);

//fixed point calcultation
#define F (1<<14)
////////////////
int i_to_f(int n);
int f_to_i(int x);
int f_to_i_round(int x);
int add_ff(int x, int y);
int add_fi(int x, int y);
int sub_ff(int x, int y);
int sub_fi(int x, int y);
int mul_ff(int x, int y);
int mul_fi(int x, int y);
int div_ff(int x, int y);
int div_fi(int x, int y);

// for mlfqs
void cal_ml_pri(struct thread *t);
void cal_ml_recent_cpu(struct thread *t);
//void update_pri(struct thread *t, void *aux);
//void update_recent_cpu(struct thread *t, void *aux);

void update_pri(void);
void update_recent_cpu(void);
void cal_load_avg(void);
//////////////////////////////////////////////////
#endif /* threads/thread.h */
