#ifndef VM_FRAME_H
#define VM_FRAME_H



#include <list.h>
#include <stdint.h>
#include <stdbool.h>
#include "threads/palloc.h"
#include "threads/synch.h"

#include "vm/page.h"


struct lock frame_table_lock;


//frame table이다
struct list frame_table;

struct frame_entry{
//	time_t finish; // 마지막으로 access된 시간
//lru 구현 frame_table을 queue처럼 관리
	
	void *paddr; //physical adress
	void *vaddr; //virtual address
	bool evictable; //second chane에서 줄 것으로 victim의 대상이 될 수있는지를 확인한다
	bool writable ;

	void *frame; //virtaul address
	struct page *page; //mapping 되어있는 page
	struct thread *thread; //해당 page가 어느 process thread의 영역인지 확인한다
	struct list_elem elem; //이러한 구조를 physical memory의 frame을 list로 구성한다

};


struct frame_entry *victim_frame;

void frame_table_init(void);
void *frame_alloc(enum palloc_flags flag, struct page *pg);
void frame_free(void *frame);
void frame_add_to_table( void *frame, struct page *pg);
void* frame_victim (enum palloc_flags flag);

struct frame * search_frame_table(void *frame);
#endif
