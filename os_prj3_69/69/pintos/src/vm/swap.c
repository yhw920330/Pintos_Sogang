#include"vm/swap.h"

struct block *swap_space;

#define USED false
#define EMPTY true

void swap_init(void)
{
	swap_space= block_get_role(BLOCK_SWAP);
	swap_bitmap= bitmap_create(swap_size_in_page());

	bitmap_set_all(swap_bitmap,EMPTY);
	lock_init(&swap_lock);

}

size_t swap_out(void *frame){
	lock_acquire(&swap_lock);
size_t put_here = bitmap_scan(swap_bitmap,0,1,true);
	if(put_here ==BITMAP_ERROR)
		return BITMAP_ERROR;
	bitmap_flip(swap_bitmap,put_here);

	size_t cnt=0;
	for(cnt=0; cnt<SEC_IN_PAGE ; cnt++)
	{
		block_write(swap_space, put_here*SEC_IN_PAGE+cnt,(uint8_t*) frame+ cnt*BLOCK_SECTOR_SIZE);
	
	}
	lock_release(&swap_lock);
	return put_here;
}

//swap space에서 memory로 가져옴
void swap_in(size_t used_index, void *frame)
{
	lock_acquire(&swap_lock);
     size_t cnt=0;
	 for(cnt=0; cnt< SEC_IN_PAGE; cnt++)
	 {
		block_read(swap_space, used_index*SEC_IN_PAGE + cnt ,(uint8_t*) frame+cnt*BLOCK_SECTOR_SIZE);
	 }

	 bitmap_flip(swap_bitmap, used_index);
	 lock_release(&swap_lock);
}

size_t swap_size_in_page(void)
{
	return block_size(swap_space) / SEC_IN_PAGE;
}
