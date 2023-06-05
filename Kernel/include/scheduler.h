#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdlib.h>
#include <defs.h>
#include <stdint.h>
#include <pipes.h>
#include <stdio.h>
#include <naiveConsole.h>

#define MAX_ARGS 5
#define MAX_ARG_LENGTH 20

#define STDIN 1
#define STDOUT 0

typedef struct fd {
  unsigned int id;
  char readable;
  char writable;
  pipe pipe;
}fd;

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
  unsigned int next_fd_id;
  fd * stdin_fd;
  fd *stdout_fd;
  fdNode *fds;
  fdNode * last_node;
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
void close_fd(unsigned int fd, int pid);
fd * create_fd(int pid);
int dup_fd(unsigned int dest_fd, unsigned int src_fd, int pid);
fd * get_stdin(int pid);
fd * get_stdout(int pid);
fd * get_fd(int pid, unsigned int searching_fd);
#endif