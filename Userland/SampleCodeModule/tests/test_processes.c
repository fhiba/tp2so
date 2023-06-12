// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscallslib.h>
#include <test_util.h>

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;



int64_t test_processes(uint64_t argc, char argv[5][20]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 2){
    printerr("Wrong amount of arguments\n");
    sys_kill(sys_get_pid());
  }

  printf(argv[1]);
  printf("\n");

  if ((max_processes = satoi(argv[1])) <= 0){
    printerr("max processes is less than 0\n");
    sys_kill(sys_get_pid());
  }

  p_rq p_rqs[max_processes];

  while (1) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
        char process_name[13] = "enldess loop"; 
        strcpy(argv[0],process_name);
        p_rqs[rq].pid = sys_process(&endless_loop,1,1,(char **)argv,NULL,NULL);

      if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    print_ps();
    sys_sleep(3000);
    printf("\n");

    printFirst("RANDOM KILL BLOCK OR UNBLOCK PROCESS\n");
    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (sys_kill(p_rqs[rq].pid) == 1) {
                printf("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = KILLED;
              printFirst("RANDOM ACTION = KILLED\n");
              alive--;
            }
                print_ps();
                sys_sleep(3000);
                printf("\n");
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (sys_block(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR blocking process\n");
                return -1;
              }
              printFirst("RANDOM ACTION = BLOCKED\n");
              p_rqs[rq].state = BLOCKED;
            }
                print_ps();
                sys_sleep(3000);
                printf("\n");
            break;
        }
      }
        print_ps();
        sys_sleep(3000);
        printf("\n");

      printFirst("RANDOM ACTION = UNBLOCKED\n");
      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (sys_block(p_rqs[rq].pid) == -1) {
            printf("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
              print_ps();
              printf("\n");
              sys_sleep(3000);
        }
    }

  }
  sys_kill(sys_get_pid());
}
