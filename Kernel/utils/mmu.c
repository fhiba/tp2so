#ifdef MMU

#include <mmu.h>
#include <stdint.h>

#define MINIMUM_BLOCK_SIZE ((unsigned int)(STRUCT_SIZE * 2))


typedef struct mem_block{
    struct mem_block * next;
    unsigned int block_size;
}mem_block;


typedef struct mem_manager{
    mem_block start;
    mem_block end;
    unsigned int remaining_space
} mem_manager;

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

    return my_mmu;
}

static void insert_block_into_free_list(mmu my_mmu, mem_block * my_memblock){
    mem_block * block_iter = &my_mmu->start;
    unsigned int insert_size = my_memblock->block_size;

    while(block_iter->next->block_size < insert_size){
        block_iter = block_iter->next;
    }

    my_memblock->next = block_iter->next;
    my_memblock->next = my_memblock;
}

void * my_malloc(mmu const my_mmu, unsigned int mem_to_alloc){
    mem_block * current;
    void * out = NULL;

    if(mem_to_alloc == 0)
        return out;
    
    
    mem_to_alloc += STRUCT_SIZE;

    if(mem_to_alloc & MASK_BYTE_ALIGMENT != 0){
        mem_to_alloc += (BYTE_ALIGMENT - (mem_to_alloc & MASK_BYTE_ALIGMENT));
    }

    if(mem_to_alloc < TOTAL_HEAP_SIZE){
        mem_block *prev = &my_mmu->start;
        current = my_mmu->start.next;

        while((current->block_size < mem_to_alloc) && current->next != NULL){
            prev = current;
            current = current->next;
        }
        if (current == &my_mmu->end)
            return out;
        out = (void *)(((uint8_t*)current) + mem_to_alloc);

        prev->next = current->next;

        if((current->block_size - mem_to_alloc) > MINIMUM_BLOCK_SIZE){
            mem_block * new =  (void * ) (((uint8_t *)current) + mem_to_alloc);
            new->block_size = current->block_size - mem_to_alloc;
            current->block_size = mem_to_alloc;
            insert_block_into_free_list(my_mmu,new);
        }   

        my_mmu->remaining_space -= current->block_size;
    }
    return out;
}

void my_free(mmu my_mmu, void * block){
    if(block == NULL)
        return;
    
    uint8_t * amount = ((uint8_t *) block);
    mem_block * aux;

    amount -= STRUCT_SIZE;

    aux = (void *)amount;

    insert_block_into_free_list(my_mmu,((mem_block *)aux));
    my_mmu->remaining_space += aux->block_size;
}

unsigned int heapSize(){
    return TOTAL_HEAP_SIZE;
}

unsigned int heapLeft(mmu my_mmu){
    return my_mmu->remaining_space;
}

unsigned int usedHeap(mmu my_mmu){
    return heapSize()-heapLeft(my_mmu);
}

#endif