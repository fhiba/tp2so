#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <defs.h>
#include <stdint.h>

#define MAX_ARGS 5
#define MAX_ARG_LENGTH 20

typedef struct fd {
  char readable;
  char writable;
  struct pipe *pipe;
  struct shm *shared_mem;
} fd;

typedef struct fdNode {
  fd *file_descriptor;
  struct fdNode *next;
}fdNode;

typedef struct pcb {
  char args[MAX_ARGS][MAX_ARG_LENGTH];
  uint32_t pid;
  uint8_t state;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t stackPointer;
  uint64_t basePointer;
  uint64_t processMemory;
  fd *stdin;
  fd *stdout;
  fdNode *fds;
  uint8_t background;
} pcb;


int create_process(uint64_t ip, uint8_t priority, uint64_t argc, char argv[MAX_ARGS][MAX_ARG_LENGTH], fd *customStdin, fd *customStdout, uint8_t background);
int switch_context(int rsp);
int kill_process(int process_id);
int get_PID();
// pcb* get_process_list();
int change_priority(int process_id, int priority);
int block_process(int process_id);
int unblock_process(int process_id);
int cede_cpu(int process_id);
int wait_pid(int process_id);
int fork();
void get_process_list();

#endif