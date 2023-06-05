#include <my_semaphore.h>


typedef struct sem{
    uint64_t id;
    atomic_int value;
    atomic_flag mutex;
} sem;

#define acquire(m) while (atomic_flag_test_and_set(m));
#define release(m) atomic_flag_clear(m);
#define MAX_SEMS 10

uint8_t sem_mut = 0;
sem * semaphores[MAX_SEMS];
int amount = 0;

atomic_int get_value(my_sem semaphore) {
    return semaphore->value;
}


sem * create_sem(){
    if(amount == MAX_SEMS){
        return 1;
    }
    semaphores[amount] = (my_sem)(alloc(sizeof(sem)));
    aquire(semaphores[amount]->mutex);
    semaphores[amount]->value = ATOMIC_VAR_INIT(1);
    semaphores[amount]->id = amount;
    return semaphores[amount++];
}

my_sem my_sem_open(my_sem sem_id){
    while(_xchg(&sem_mut,1) != 0);

    if(sem_id->id >= amount){
        my_sem out = create_sem();
        _xchg(&sem_mut,0);
        return out;
    }
    _xchg(&sem_mut,0);
    return semaphores[sem_id->id];
}

int my_sem_close(my_sem close){
    while(_xchg(&sem_mut,1) != 0);

    if(close->id >= amount){
        return;
    }

    int aux = 0;
    while (aux < amount && semaphores[aux]->id != close->id) {
        aux++;
    }

    if (aux == amount) {
        _xchg(&sem_mut, 0);
        return -1;
    }

    while (aux < amount - 1) {
        semaphores[aux] = semaphores[aux + 1];
        aux++;
    }

    amount--;
    free(close);
    _xchg(&sem_mut,0);
    return 1;
}

int my_sem_wait(sem * sem_id){
    acquire(&sem_id->mutex);                 
    while (atomic_load(&sem_id->value) <= 0); //not sure si va un mayor
    atomic_fetch_sub(&sem_id->value, 1);
    release(&sem_id->mutex);
    return 0;
}

int my_sem_post(sem * sem_id){
  return atomic_fetch_add(&sem_id->value, 1);
}