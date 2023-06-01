#ifndef MY_SEMPAHORE_H
#define MY_SEMAPHORE_H

typedef struct sem * my_sem;

int my_sem_post(my_sem sem);

int my_sem_wait(my_sem sem);

int my_sem_close(my_sem sem);

int my_sem_open(my_sem sem);


my_sem create_sem();

#endif