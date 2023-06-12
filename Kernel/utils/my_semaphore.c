// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <my_semaphore.h>
#include <scheduler.h>
#include <lib.h>
#define acquire(m) while (atomic_flag_test_and_set(m));
#define release(m) atomic_flag_clear(m);
#define MAX 10

typedef struct sem{
    uint64_t id;
    int value;
    uint32_t waiting;
    uint8_t mutex;
    pcb * processes[MAX];
} sem;



uint8_t sem_mut = 0;
sem * semaphores[MAX];
int amount = 0;

atomic_int get_value(my_sem semaphore) {
    return semaphore->value;
}


sem * create_sem(){
    if(amount == MAX){
        return NULL;
    }
    semaphores[amount] = (my_sem)(alloc(sizeof(sem)));
    acquire(&semaphores[amount]->mutex);
    semaphores[amount]->value = 1;
    semaphores[amount]->id = amount;
    return semaphores[amount++];
}

my_sem my_sem_open(uint64_t id){
    while (_xchg(&sem_mut, 1) != 0);

    if(amount == MAX)
        return NULL;

    int semIter = 0;
    while (semIter < amount && semaphores[semIter]->id != id) {
        semIter++;
    }

    

    if (semIter == amount) {
        semaphores[semIter] = (sem *)alloc(sizeof(sem));
        semaphores[semIter]->id = id;
        semaphores[semIter]->waiting = 0;
        semaphores[semIter]->value = 1;
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
        if(sem_id->waiting == MAX){
            return -1;
        }

        sem_id->processes[sem_id->waiting++] = block_process(get_PID());


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

    sem_id->processes[0]->state = 1;
    sem_id->waiting--;

    for( int i = 0; i < sem_id->waiting;i++){
        sem_id->processes[i] = sem_id->processes[i + 1];
    }

    _xchg(&sem_id->mutex, 0);
    return 0;
}

void remove_process_sem(int64_t pid) {
  for (int i = 0; i < amount; i++) {
    for (int j = 0; j < semaphores[i]->waiting; j++) {
      if (semaphores[i]->processes[j]->pid == pid) {
        semaphores[i]->waiting--;

        for (int k = j; k < semaphores[i]->waiting; k++) {
          semaphores[i]->processes[k] = semaphores[i]->processes[k + 1];
        }
        return;
      }
    }
  }
}