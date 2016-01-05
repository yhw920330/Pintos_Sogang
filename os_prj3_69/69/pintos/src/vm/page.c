#include"page.h"
#include<stdio.h>
#include<string.h>
#include<stdbool.h>


#include"userprog/pagedir.h"
#include"userprog/process.h"
#include"userprog/syscall.h"


#include"filesys/file.h"

#include"threads/interrupt.h"
#include"threads/malloc.h"
#include"threads/palloc.h"
#include"threads/vaddr.h"
#include"threads/thread.h"
#include"threads/synch.h" //sturct lock
#include "vm/frame.h"
#include "vm/swap.h"

///////prototype
static bool page_less_func(const struct hash_elem *a, 
		const struct hash_elem *b,
		void *aus UNUSED);

void page_each_free(struct hash_elem *e, void *aux UNUSED);










//page table을 free시켜준다
void page_table_init(struct hash *pt){
	hash_init(pt, page_each_free,page_less_func, NULL);
}

//page table을 없애는 과정
void page_table_destory(struct hash *pt){

	hash_destroy(pt, page_each_free);
}

//load의 wrapper함수 이다
//page의 종류에 따라서 load함수를 호출한다
bool load_page(struct page *pt_entry)
{
	bool done=false;
	pt_entry ->pinned =true;

	//이미 올라가있는 page이므로
	if(pt_entry ->valid == true)
	{
		return false;
	}

	//file일때
	if(pt_entry-> type ==0)
	{
		done= load_file(pt_entry);
	}

	//swap일때
	else if( pt_entry -> type==1)
	{
		done=load_swap(pt_entry);
	}


	return done;




}
void set_pinned_false(struct page *pt_entry)
{
	pt_entry -> pinned =false;

}
bool load_swap(struct page *pt_entry){

	bool done=false;
	uint8_t *frame= frame_alloc(PAL_USER,pt_entry);


	//install_page는 process.c에있는 함수
	//install_page(user vaddr -> kernel page로 mapping)

	if(frame ==NULL)
	{
		return false;
	}
	else
	{
		done=install_page(pt_entry->vaddr,frame, pt_entry->writable);

		//mapping 실패시 종료
		if(done==false){
			frame_free(frame);
			return done;
		}

		//mapping 성공
		swap_in(pt_entry->swap_index,pt_entry->vaddr);
		pt_entry->valid=true;
		return true;
	}


}
bool load_file(struct page *pt_entry){

	//palloc = page allocate
	//palloc flag 
	//PAL_ASSERT(PANIC ON FAILURE)   001
	//PAL_ZEOR - ZERO PAGE CONTENT   010
	//PAL_USER - USER PAGE           100
	bool done=false;
	uint8_t *frame=NULL;
	enum palloc_flags flag=PAL_USER;
	struct lock f_lock;

	if(pt_entry ->read_bytes ==0)
	{
		flag+=PAL_ZERO; //flag가 110 6이됨
	}
	frame=frame_alloc(flag,pt_entry);

	if(frame ==NULL){
		return done;

	}
	//file read 부분
	lock_acquire(&f_lock);

		///check 추가해야함
	file_read_at(pt_entry->file, frame,
			pt_entry->read_bytes, pt_entry->offset);

	memset(frame+pt_entry->read_bytes,0,pt_entry->zero_bytes);
	lock_release(&f_lock);

	//////////////////

	done=install_page(pt_entry->vaddr, frame,pt_entry->writable);
	
	if(done==false)
	{
		frame_free(frame);
		return false;
	}

	pt_entry->valid=true;
	return true;


}

bool add_file_on_page_table(struct file *file, int32_t offset,
		uint8_t *page, uint32_t ready_bytes, uint32_t zero_bytes,
		bool writable){

	bool done;

	struct page *p=malloc(sizeof(struct page));
	p->file=file;
	p->offset=offset;
	p->vaddr=page;
	p->read_bytes=ready_bytes;
	p->zero_bytes= zero_bytes;

	p->valid=false;
	p->type=0; //file로 type
	p->pinned=false;

	//hash insert 함수는 이미 그값이 없으면 null을 return
	if( hash_insert(&thread_current()->pt, &p->elem)==NULL)
	{
		return true;
	}
	else
		return false;

		
	


}

bool grow_stack(void *vaddr){

 size_t len= PHYS_BASE - pg_round_down(vaddr);

 if( len > MAX_STACK_SIZE)
 {
	 return false;
 }

 struct page *pg= malloc(sizeof(struct page));
 pg->vaddr = pg_round_down(vaddr);
 pg->valid=true;
 pg->writable=true;
 pg->type=1; //swap type으로 해줌
 pg->pinned = true;


 uint8_t *frame = frame_alloc(PAL_USER,pg);

if( !install_page(pg->vaddr,frame ,pg->writable))
{
	free(pg);
	frame_free(frame);
	return false;
}

if(intr_context())
{
	pg->pinned = false;
}

if(hash_insert( & thread_current()->pt,&pg->elem) == NULL)
	return true;
else
	return false;

}


//page table의 entry를 할당받는 함수
struct page* get_page_entry(void *vaddr)
{
	struct page pg;
	pg.vaddr = pg_round_down(vaddr);

	struct hash_elem *he= hash_find(&thread_current()->pt,&pg.elem);
	if(he!=NULL)
	{
		return hash_entry(he,struct page, elem);
	
	}

	return NULL;


}


//page의 주소를 hash funct값으로한다
static unsigned page_hash_func(const struct hash_elem *e, void *aus UNUSED)
{
	struct page *pg= hash_entry(e,struct page , elem);
	return hash_int((int) pg->vaddr);

}

//주소값을 비교한다
static bool page_less_func(const struct hash_elem *a, 
		const struct hash_elem *b,
		void *aus UNUSED)
{
	struct page *p1 = hash_entry(a, struct page, elem);
	struct page *p2 = hash_entry(b, struct page, elem);
	if( p1->vaddr < p2->vaddr)
		return true;
	else
		return false;
}

void page_each_free(struct hash_elem *e, void *aux UNUSED)
{
	struct page *pg=hash_entry(e,struct page, elem);
	if(pg->valid)//physical에 올라가있는 대상이라면
	{
		//physical 을 free해준다
		frame_free(pagedir_get_page(thread_current()->pagedir, pg->vaddr));
		//p
		pagedir_clear_page(thread_current()->pagedir, pg->vaddr);

		

	}
	//해당 page를 free시켜준다
	free(pg);

}
