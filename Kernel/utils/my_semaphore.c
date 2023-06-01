#include <my_semaphore.h>
#include <mutex.h>
#include <mmu_wrapper.h>
#include <stdatomic.h>
#include <stdint.h>
#include <asm/unistd.h>

typedef struct sem{
    uint64_t id;
    atomic_int value;
    atomic_flag mutex;
    boolean 
} sem;

#define acquire(m) while (atomic_flag_test_and_set(m));
#define release(m) atomic_flag_clear(m);
#define MAX_SEMS 10

uint8_t sem_mut = 0;
sem * semaphores[MAX_SEMS];
int amount = 0;

sem * create_sem(){
    if(amount == MAX_SEMS){
        exit(1);
    }
    semaphores[amount] = (my_sem)(alloc(sizeof(sem)));
    // semaphores[amount]->mutex = ;
    semaphores[amount]->value = ATOMIC_VAR_INIT(0);
    semaphores[amount]->id = amount;
    
    return semaphores[amount++];
}

sem * my_sem_open(uint64_t id){
    while(_xchg(&sem_mut,1) != 0);

    if(id >= amount){
        return create_sem();
    }
    _xchg(&sem_mut,0);
    return semaphores[id];
}

void my_sem_close(uint64_t id){
    while(_xchg(&sem_mut,1) != 0);

    if(id >= amount){
        return;
    }

    _xchg(&sem_mut,0);
}

int my_sem_wait(sem * sem_id){
    acquire(&sem_id->mutex);                 
    while (atomic_load(&sem_id->value) <= 0); //not sure si va un mayor
    atomic_fetch_sub(&sem_id->value, 1);
    release(&sem_id->mutex);
    return 0;
}

void my_sem_post(sem * sem_id){
  return atomic_fetch_add(&sem_id->value, 1);
}