#ifndef VM_PAGE_H
#define VM_PAGE_H

#include"devices/block.h"
#include"filesys/off_t.h"
#include"threads/synch.h"
#include<hash.h>
#include "vm/frame.h"


#define MAX_STACK_SIZE 1<<23
//8M = 2^23 

struct page{
	int type;//file-0 swap-1 
	void *vaddr; // virtual address
	struct hash_elem elem; //해시로 page table을 관리하기 위해서이다
	struct file *file;
	bool mode_bit;

	///////////////////////////
 

	bool writable; // read only와 구분하기 위해서 사용
	bool valid;    // physical memory에 load의 여부를 나타내기 위해서 사용
	bool pinned;
	
	size_t offset;
	size_t read_bytes;
	size_t zero_bytes;

	size_t swap_index;
};

//thread_start에서 호출 되어야한다(미구현)
void page_table_init(struct hash *spt);  //page table을 초기화한다
//process _exit에서 호출 되어야한다( 미구현)
void page_table_destory(struct hash *spt); //page table을 destory한다

//load page의 부분으로 page는 swap이 될 수도있고 file이 될 수 도있다.
bool load_page(struct page *pt_entry);
bool load_swap(struct page *pt_entry);
bool load_file(struct page *pt_entry);


bool add_file_on_page_table(struct file *file, int32_t offset,
		uint8_t *page, uint32_t ready_bytes, uint32_t zero_bytes,
		bool writable);

//grow_stack으로 user영역을 확장해주는 함수이다

bool grow_stack(void *vaddr);
struct page* get_page_entry(void *vaddr);


void set_pinned_false(struct page *pt_entry);

#endif
