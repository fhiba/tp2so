#ifndef MMU_H
#define MMU_H

#define NULL ((void *)0)
#define BYTE_ALIGMENT 8
#define MASK_BYTE_ALIGMENT 0x07
#define TOTAL_HEAP_SIZE 134217728 // 128MB

typedef struct mem_manager * mmu;

void * my_malloc(mmu const my_mmu, unsigned int mem_to_alloc );//pasarle el memory manager y la cantidad

void my_free(mmu const my_mmu, void * block);//recibe mem manager + bloque(?)

unsigned int heapSize();

unsigned int heapLeft(mmu my_mmu);//recibe el memory manager

unsigned int usedHeap(mmu my_mmu);//recibe el memory manager

#endif