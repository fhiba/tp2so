// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
// #include <stdio.h>
#include <syscallslib.h>
#include <programs.h>
#include <test_util.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void print_ps(){
  char buffer[1024] = {0};
  sys_ps(buffer,NULL);
  printf(buffer);
}

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;
  char process_name[13] = "enldess loop"; 
  strcpy(argv[0],process_name);
  for (i = 0; i < TOTAL_PROCESSES; i++){
    pids[i] = sys_process(&endless_loop,1,1,(char **)argv,NULL,NULL);
  }

  printFirst("INITIAL PS\n");
  print_ps();


  printFirst("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){

    sys_nice((int)pids[i], (int)prio[i]);
    printf("\n");
    print_ps();
  }

  sys_sleep(3000);


  printFirst("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){
    sys_block(pids[i]);
    printf("\n");
    print_ps();
  }
  sys_sleep(3000);

  printFirst("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){
    sys_nice((int)pids[i], (int)prio[i]);
    print_ps();
  }

  sys_sleep(3000);
  printFirst("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){
    sys_block(pids[i]);
    print_ps();  
  }

  sys_sleep(1000);

  printFirst("\nKILLING...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++){
    sys_kill(pids[i]);
    print_ps();
  }
  sys_kill(sys_get_pid());
}
