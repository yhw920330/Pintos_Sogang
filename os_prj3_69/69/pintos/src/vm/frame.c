#include "filesys/file.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "vm/frame.h"

#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"

#include "userprog/pagedir.h"

void frame_table_init(void){

	list_init(&frame_table);
	lock_init(&frame_table_lock);
	victim_frame = NULL;
	
}

void *frame_alloc(enum palloc_flags flag, struct page *pg){

	if(flag<4 ) //user 영역이 아님
		return NULL;

	void *frame = palloc_get_page(flag);

	if( frame)
	{
		frame_add_to_table(frame, pg);
	}
	else
	{
		while(!frame)
		{
			frame=frame_victim(flag);
			lock_release(&frame_table_lock);

		}
		
		frame_add_to_table(frame, pg);
	}
	return frame;

}



void frame_free(void *frame){
	struct list_elem *e;

	lock_acquire(&frame_table_lock);
	for(e= list_begin(&frame_table);
			e!= list_end(&frame_table);
			e= list_next(e))
	{

		//searh 함수 만들기

		struct frame_entry *f= list_entry(e, struct frame_entry, elem);

		if(f->frame == frame)
		{

			list_remove(e);
			free(f);
			palloc_free_page(frame);
			break;
		}

	}

	lock_release(&frame_table_lock);


}


void frame_add_to_table( void *frame, struct page *pg)
{

	struct frame_entry *f = malloc(sizeof(struct frame_entry));
	lock_acquire(&frame_table_lock);
	f->frame= frame;
	f->page= pg;
	f->thread = thread_current();
	list_push_back(&frame_table, &f->elem);

	lock_release(&frame_table_lock);


}
void* frame_victim (enum palloc_flags flag)
{

	lock_acquire(&frame_table_lock);


	struct list_elem *e= list_begin(&frame_table);

//list_
	
	
	victim_frame=list_pop_front(&frame_table);
	if(victim_frame ==NULL)
	{
	}
	else
	{
		victim_frame->page->valid=false;
		pagedir_clear_page(victim_frame->thread->pagedir, victim_frame->page->vaddr);
		palloc_free_page(victim_frame->frame);
		return palloc_get_page(flag);
	}

	lock_release(&frame_table_lock);

}

struct frame * search_frame_table(void *frame){

	struct list_elem *update;
	struct list_elem *e;
	struct frame_entry  *tp;
	for(e=list_begin(&frame_table); e!=list_end(&frame_table); e=list_next(e))
	{
		tp=list_entry(e, struct frame_entry, elem);
		if(frame== tp->paddr){
			update=list_remove(&(tp->elem));
			list_push_back(&frame_table,update); //update
		   return tp;
		}
	}
	return NULL;
}

