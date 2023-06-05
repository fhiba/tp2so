#include <ourlib.h>
#include <phylo.h>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + n - 1) % n
#define RIGHT (phnum + 1) % n
#define GENERALSEMID 100
#define MAX_PHILOSOPHERS 10

void printTable();

int state[MAX_PHILOSOPHERS];
int pids[MAX_PHILOSOPHERS];
int n = N;

my_sem mutex;
my_sem chopsticks[N];

void test(int phnum) {
  if (state[phnum] == HUNGRY && state[LEFT] != EATING &&
      state[RIGHT] != EATING) {
    state[phnum] = EATING;
    printTable();
    semPost(chopsticks[phnum]);
  }
}

void take_fork(int phnum) {
  semWait(mutex);

  state[phnum] = HUNGRY;

  test(phnum);

  sem_post(mutex);

  sem_wait(chopsticks[phnum]);

  sleep(1);
}

void put_fork(int phnum) {
  sem_wait(mutex);

  state[phnum] = THINKING;

  test(LEFT);
  test(RIGHT);

  sem_post(mutex);
}

void * phylo(int argc, char argv[MAX_ARGS][MAX_ARG_LENGTH]) {
  if (argc != 2) {
    exit();
  }

  int num = atoi(argv[1]);
  while (1) {
    sleep(1000);

    take_fork(num);

    sleep(1000);

    put_fork(num);
  }
}

void philosophersProgram() {
  int i;
  mutex = sem_open(GENERALSEMID, 1);

  for (i = 0; i < N; i++) {
    chopsticks[i] = sem_open(i, 1);
  }

  char args[MAX_ARGS][MAX_ARG_LENGTH];
  char num[3];
  strcpy(args[0], "philo");
  for (i = 0; i < N; i++) {
    uintToBase(i, num, 10);
    strcpy(args[1], num);
    pids[i] = createProcess((uint64_t)phylo, 3, 2, args, NULL, NULL);
  }

  char c;
  while (1) {
    getchar(&c);
    switch (c) {
      case 'a':
      case 'A':
        if (n == MAX_PHILOSOPHERS) {
          printf("The table is full, no new philosophers can join\n");
        } else {
          uintToBase(n, num, 10);
          strcpy(args[1], num);
          sem_wait(mutex);
          chopsticks[n] = sem_open(n, i);
          state[n] = THINKING;
          pids[n++] = createProcess((uint64_t)phylo, 3, 2, args, NULL, NULL);
          sem_post(mutex);
          printf("The philosopher sat on the table\n");
        }
      break;
      case 'r':
      case 'R': {
        if (n <= 2) {
          printf("No more philosophers can leave\n");
        } else {
            sem_wait(mutex);
            kill(pids[n - 1]);
            n--;
            printf("The philosopher left the rift\n");
            sem_post(mutex);
          }
      }
      break;
      default:
        break;
    }
  }
  exit();
}

void printTable() {
  int i;
  for (i = 0; i < n; i++) {
    if (state[i] == EATING) {
      printf("E ");
    } else {
      printf(". ");
    }
  }
  putchar('\n');
}