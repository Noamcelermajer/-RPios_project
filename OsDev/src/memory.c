#include <stdint.h>
#include "memory.h"
#include "printf.h"
#include "strings.h"

typedef struct mem_block {
  uint32_t size;
  struct mem_block* next_block;
} mem_block;

static mem_block* block_list_head;


//take a head ptr and transform it to a block
mem_block* blockify(uint32_t head_ptr, uint32_t given_size) {
  mem_block* block = (mem_block*) head_ptr;
  block->next_block = NULL;
  block->size = given_size - sizeof(mem_block);
  return block;
}
//init heap
void init_memory(uint32_t mem_ptr, uint32_t size) {
  block_list_head = blockify(mem_ptr, size);
  printf("Memory initialized at %d to %d for a total of : %d\n",mem_ptr,size,size - mem_ptr);
}
//add a new block
void insert_new_block(mem_block* block) {
  block->next_block = block_list_head;
  block_list_head = block;
}

//split block to needed size
void split(mem_block* block, uint32_t req_size) {
  uint32_t new_prt = ((uint32_t) block) + sizeof(mem_block) + req_size;
  uint32_t new_avail_size = block->size - req_size - sizeof(mem_block);
  mem_block* new_block = blockify(new_prt, new_avail_size);
  insert_new_block(new_block);
  block->size = req_size;
}
//check if block is free
int is_free(mem_block* block) {
  return (block->size & 0x1) == 0;
}


void toggle_alloc(mem_block* block) {
  block->size ^= 0x1;
}


void* malloc(uint32_t size) {
  // round up to nearest multiple of 16
  size = ((size >> 4) + 1) << 4;

  mem_block* block = block_list_head;
  while(block) {
    if (is_free(block) && block->size >= size) {
      if (block->size >= size+sizeof(mem_block)+16) {
        split(block, size);
      }
      toggle_alloc(block);
      return (void*) (((uint8_t*) block) + sizeof(mem_block));
    }
    block = block->next_block;
  }
  return NULL;

  // really simple
  // size = ((size >> 4) + 1) << 4;

  // void* ret = (void*) heap_ptr;
  // heap_ptr += size;
  // return ret;
}

void free(void* ptr) {
  if (ptr != NULL) {
    memset(ptr,0,strlen((char*)ptr));
    mem_block* block = (mem_block*) (((uint8_t*) ptr) - sizeof(mem_block));
    toggle_alloc(block);
  }
}

void dump() 
{
  mem_block* block = block_list_head;
  int i = 0 , allocated_block  =0;
  while(block)
  {
    if (!is_free(block)) 
    {
      i++;
      allocated_block++;
      block =  block->next_block;

    }
    else
    {
     block =  block->next_block;
  i++;
    }

 
  }

      printf("the number of allocated block is %d - max available memory is : %d \n",allocated_block,i );
}
