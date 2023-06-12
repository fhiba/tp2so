// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <ourlib.h>
#include <syscallslib.h>
#include <test_util.h>

#define SEM_ID 1
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

  printf("entre\n");

  if (argc != 3)
    sys_kill(sys_get_pid());

  if ((n = satoi(argv[0])) <= 0){
    printerr("Wrong value for n\n");
    sys_kill(sys_get_pid());
  }
  if ((inc = satoi(argv[1])) == 0){
    printerr("Wrong value for inc\n");
    sys_kill(sys_get_pid());
  }

  my_sem test_sem = sys_sem_open(SEM_ID);
  if (use_sem)
    if (test_sem == NULL) {
      printf("test_sync: ERROR opening semaphore\n");
      sys_kill(sys_get_pid());
    }
  
  char num[5];
  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sys_sem_wait(test_sem);
    slowInc(&global, inc);
    intToString(global,num);
    printf(num);
    printf("\n");
    sys_kill(1000);
    if (use_sem)
      sys_sem_post(test_sem);
  }

  if (use_sem)
    sys_sem_close(test_sem);

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2){
    printerr("Wrong amount of arguments\n");
    sys_kill(sys_get_pid());
  }
  printf("N = ");
  printf(argv[1]);
  printf("\n");

  char *argvDec[] = {argv[0], "-1", argv[1]};
  char *argvInc[] = {argv[0], "1", argv[1]};

  global = 0;
  printf("Creating childs\n");
  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = sys_create_child(sys_get_pid(),&my_process_inc,5, 3, argvDec,NULL,NULL);
    pids[i + TOTAL_PAIR_PROCESSES] = sys_create_child(sys_get_pid(),&my_process_inc,5, 3, argvInc,NULL,NULL);
  }
  printf("Childs created\n");

  printf("Waiting for pids\n");
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    sys_wait_pid(sys_get_pid());
  }

  printf("Final value: ");
  char num[10];
  intToString(global,num);
  printf(num);
  printf("\n");
  sys_kill(sys_get_pid());
}
