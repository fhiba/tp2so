#include <mmu_wrapper.h>
#include <mmu.h>

static mmu my_mmu;

void new_mmu(void * const restrict mem_to_assign, void * const restrict managed_mem){
    my_mmu = create_memory_management(mem_to_assign,managed_mem);
}

void * alloc(unsigned int mem_to_alloc){
    my_malloc(my_mmu,mem_to_alloc);
}

void free(void * mem_to_free){
    my_free(my_mmu,mem_to_free);
}

void mem_status(unsigned int * status){
    status[0] = heapSize();
    status[1] = heapLeft(my_mmu);
    status[2] = usedHeap(my_mmu);
}