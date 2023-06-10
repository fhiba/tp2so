// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef MY_SEMPAHORE_H
#define MY_SEMAPHORE_H

#include <stddef.h>
#include <mutex.h>
#include <mmu_wrapper.h>
#include <stdatomic.h>
#include <stdint.h>
#include <asm/unistd.h>

extern uint8_t _xchg(uint8_t * lock,int value);

typedef struct sem * my_sem;

int my_sem_post(my_sem sem);

int my_sem_wait(my_sem sem);

int my_sem_close(my_sem sem);

my_sem my_sem_open(my_sem sem);

atomic_int get_value(my_sem semaphore);

my_sem create_sem();

#endif