#include <my_semaphore.h>
#include <lib.h>
#define acquire(m) while (atomic_flag_test_and_set(m));
#define release(m) atomic_flag_clear(m);
#define MAX_SEMS 10

typedef struct sem{
    uint64_t id;
    int value;
    uint32_t waiting;
    uint8_t mutex;
} sem;



uint8_t sem_mut = 0;
sem * semaphores[MAX_SEMS];
int amount = 0;

atomic_int get_value(my_sem semaphore) {
    return semaphore->value;
}


sem * create_sem(){
    if(amount == MAX_SEMS){
        return NULL;
    }
    semaphores[amount] = (my_sem)(alloc(sizeof(sem)));
    acquire(&semaphores[amount]->mutex);
    semaphores[amount]->value = 1;
    semaphores[amount]->id = amount;
    return semaphores[amount++];
}

my_sem my_sem_open(my_sem sem_id){
    while (_xchg(&sem_mut, 1) != 0);

    int semIter = 0;
    while (semIter < amount && semaphores[semIter]->id != sem_id->id) {
        semIter++;
    }

    if (semIter == amount) {
        semaphores[semIter] = (sem *)alloc(sizeof(sem));
        semaphores[semIter]->id = sem_id->id;
        semaphores[semIter]->waiting = 0;
        semaphores[semIter]->value = sem_id->value;
        _xchg(&sem_mut, 0);
        return semaphores[amount++];
    }

    _xchg(&sem_mut, 0);
    return semaphores[semIter];
}

int my_sem_close(my_sem close){
    if (close->waiting > 0) {
    return -1;
  }
    while (_xchg(&sem_mut, 1) != 0);

    int semIter = 0;
    while (semIter < amount && semaphores[semIter]->id != close->id) {
        semIter++;
    }

    if (semIter == amount) {
        _xchg(&sem_mut, 0);
        return -1;
    }
    while (semIter < amount - 1) {
        semaphores[semIter] = semaphores[semIter + 1];
        semIter++;
    }

    amount--;
    free(close);

    _xchg(&sem_mut, 0);
    return 1;
}

int my_sem_wait(sem * sem_id){
    while (_xchg(&sem_id->mutex, 1) != 0);

    while (sem_id->value == 0) {

        _xchg(&sem_id->mutex, 0);

        force_timer();
    }

    sem_id->value--;

    _xchg(&sem_id->mutex, 0);

    return 0;
}

int my_sem_post(sem * sem_id){
    while (_xchg(&sem_id->mutex, 1) != 0);

    sem_id->value++;

    if (sem_id->waiting == 0) {
        _xchg(&sem_id->mutex, 0);
        return 0;
    }

    sem_id->waiting--;

    _xchg(&sem_id->mutex, 0);
    return 0;
}