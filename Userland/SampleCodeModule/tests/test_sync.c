// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <syscallslib.h>
#include <test_util.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  sys_cede(sys_get_pid()); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

    my_sem test_sem = sys_sem_open(SEM_ID);
  if (use_sem)
    if (test_sem == NULL) {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sys_sem_wait(test_sem);
    slowInc(&global, inc);
    if (use_sem)
      sys_sem_post(test_sem);
  }

  if (use_sem)
    sys_sem_close(test_sem);

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = sys_create_child(sys_get_pid(),&my_process_inc,5, 3, argvDec,NULL,NULL);
    pids[i + TOTAL_PAIR_PROCESSES] = sys_create_child(sys_get_pid(),&my_process_inc,5, 3, argvInc,NULL,NULL);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    sys_wait_pid(pids[i]);
    sys_wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printf("Final value: ");
  char num[10];
  intToString(global,num);
  printf(num);

  return 0;
}
