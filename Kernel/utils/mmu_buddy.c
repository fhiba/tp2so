// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef BUDDY

#include <mmu.h>
#include <stdint.h>

#define MINIMUM_BLOCK_SIZE 512

typedef struct mem_block{
    unsigned char free;
    int history;
    struct mem_block * next;
    unsigned int block_size;
}mem_block;


typedef struct mem_manager{
    mem_block start;
    mem_block end;
    unsigned int remaining_space;
}mem_manager;

static const uint16_t STRUCT_SIZE = ((sizeof(mem_block) + (BYTE_ALIGMENT - 1)) & ~MASK_BYTE_ALIGMENT);

mmu create_memory_management(void * const restrict mem_to_assign, void * const restrict managed_mem ){
    mmu my_mmu = (mmu)mem_to_assign;
    my_mmu->remaining_space = TOTAL_HEAP_SIZE;

    mem_block * first_pos = (void *)managed_mem;

    my_mmu->start.next = (void*)first_pos;
    my_mmu->start.block_size = (unsigned int) 0;

    my_mmu->end.next = NULL;
    my_mmu->end.block_size = TOTAL_HEAP_SIZE;
    
    first_pos->next= &my_mmu->end;
    first_pos->block_size = TOTAL_HEAP_SIZE;

    first_pos->free = 1;
    first_pos->history = 0;

    return my_mmu;
}

static void remove_block_from_list(mmu my_mmu, mem_block * to_delete){
    mem_block * iter = &my_mmu->start;

    while(iter != NULL && iter->next != to_delete){
        iter = iter->next;
    }

    if(iter != NULL){
        iter->next = iter->next->next;
    }

}

static int insert_block_into_free_list(mmu my_mmu, mem_block * in_block, unsigned char merge){
    if(in_block->block_size == TOTAL_HEAP_SIZE){
        my_mmu->start.next = in_block;
        in_block->next = &my_mmu->end;
        in_block->free = 1;
        return in_block->block_size;
    }

    mem_block * buddy = NULL;
    int aux = 0;

    if(merge){
        if((in_block->history & 0x1) == 1){
            buddy = (mem_block *)((uint64_t)in_block - in_block->block_size);
            if(buddy->free && buddy->block_size == in_block->block_size){
                buddy->block_size *=2;
                aux = 1;
                in_block = buddy;
            }
        } else {
            buddy = (mem_block *)((uint64_t)in_block + in_block->block_size);
            if(buddy->free && buddy->block_size == in_block->block_size) {
                in_block->block_size *= 2;
                aux = 1;
            }
        }
    }
    if(aux == 1){
        in_block->history = in_block->history >> 1;
        remove_block_from_list(my_mmu,buddy);
        return insert_block_into_free_list(my_mmu,in_block,1);
    }

    mem_block * iter = &my_mmu->start;
    int block_size = in_block->block_size;

    while(iter->next->block_size < block_size){
        iter = iter->next;
    }

    in_block->next = iter->next;
    iter->next = in_block;
    in_block->free = 1;
    return in_block->block_size;
}

void * my_malloc(mmu const my_mmu, unsigned int mem_to_alloc){
    mem_block * current, * previous;
    void * out = NULL;

    if(mem_to_alloc == 0){
        return out;
    }

    mem_to_alloc += STRUCT_SIZE;

    if((mem_to_alloc & MASK_BYTE_ALIGMENT) != 0){
        mem_to_alloc += (BYTE_ALIGMENT - (mem_to_alloc & MASK_BYTE_ALIGMENT));
    }

    if(mem_to_alloc >   TOTAL_HEAP_SIZE){
        return out;
    }

    previous = &my_mmu->start;
    current = my_mmu->start.next;

    while((current->block_size < mem_to_alloc) && (current->next != NULL)){
        previous = current;
        current = current->next;
    }

    if(current ==  &my_mmu->end)
        return NULL;
    
    out = (void *)(((uint8_t *)previous->next)+ STRUCT_SIZE);
    previous->next = current->next;

    while (current->block_size/2 >= MINIMUM_BLOCK_SIZE && current->block_size /2 >= mem_to_alloc){
        current->block_size /= 2;
        current->history = current->history << 1;
        mem_block * new_block = (void *)(((uint64_t)current) + current->block_size);
        new_block->block_size = current->block_size;
        new_block->free = 1;
        new_block->history = current->history | 0x1;
        insert_block_into_free_list(my_mmu, new_block,0);
    }

    my_mmu->remaining_space -= current->block_size;
    current->free = 0;

    return out;
}

void my_free(mmu const my_mmu, void * block){
    if(block == NULL){
        return;
    }

    uint8_t * to_free =  ((uint8_t *)block);
    mem_block * block_to_free;

    to_free -= STRUCT_SIZE;

    block_to_free = (void *)to_free;
    
    unsigned int aux = block_to_free->block_size;
    insert_block_into_free_list(my_mmu,((mem_block *) block_to_free),1);
    my_mmu->remaining_space += aux;
}

unsigned int heapSize(){
    return TOTAL_HEAP_SIZE;
}

unsigned int heapLeft(mmu my_mmu){
    return my_mmu->remaining_space;
}

unsigned int usedHeap(mmu my_mmu){
    return heapSize() - heapLeft(my_mmu);
}

#endif 