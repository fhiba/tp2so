#ifndef MMU_WRAPPER_H
#define MMU_WRAPPER_H

void new_mmu(void * const restrict mem_to_assign, void * const restrict managed_mem);

void * alloc(unsigned int mem_to_alloc);

void free(void * mem_to_free);

void mem_status(unsigned int * status);

#endif