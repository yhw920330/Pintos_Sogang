#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "devices/block.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "bitmap.h"
#include "inttypes.h"
#include "stddef.h"
#include "stdbool.h"

//swap space의 bitmap
struct bitmap *swap_bitmap;
#define SEC_IN_PAGE (PGSIZE/BLOCK_SECTOR_SIZE)



void swap_init(void);
void clear_slot(size_t);
void swap_in(size_t used_index, void *frame);
size_t swap_out(void *frame);

size_t swap_size_in_page(void);

struct lock swap_lock;


#endif
