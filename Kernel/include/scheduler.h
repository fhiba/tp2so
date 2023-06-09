// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdlib.h>
#include <defs.h>
#include <stdint.h>
#include <pipes.h>
#include <stdio.h>
#include <naiveConsole.h>
#include <interrupts.h>
#include <lib.h>

#define ARG_QTY 5
#define ARG_LEN 20

#define STDIN 1
#define STDOUT 0


typedef struct fdNode {
  fd * file_descriptor;
  struct fdNode * next;
}fdNode;

typedef struct pid_node {
  int pid;
  struct pid_node *next;
}pid_node;

typedef struct pcb {
  char args[ARG_QTY][ARG_LEN];
  uint32_t pid;
  uint32_t ppid;
  uint8_t state;
  uint8_t priority;
  uint8_t auxPriority;
  uint64_t stackPointer;
  uint64_t basePointer;
  uint64_t processMemory;
  unsigned int next_fd_id;
  fd * stdin_fd;
  fd * stdout_fd;
  fdNode * fds;
  fdNode * last_node;
  uint8_t background;
  pid_node *child_pid_list;
} pcb;


int create_process(uint64_t ip, uint8_t priority, uint64_t argc, char argv[ARG_QTY][ARG_LEN], fd *customStdin, fd *customStdout);
int create_child(int ppid,uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout);
int switch_context(int rsp);
int kill_process(int process_id);
int get_PID();
// pcb* get_process_list();
void initScheduler();
int change_priority(int process_id, int priority);
int block_process(int process_id);
int unblock_process(int process_id);
int cede_cpu(int process_id);
int wait_pid(int process_id);
int fork();
void get_process_list(char *buf);
void close_fd(unsigned int fd, int pid);
fd * create_fd(int pid);
int dup_fd(unsigned int dest_fd, unsigned int src_fd, int pid);
fd * get_stdin(int pid);
fd * get_stdout(int pid);
fd * get_fd(int pid, unsigned int searching_fd);
int find_fd_id(fdNode * list, fd * my_fd);
pcb * get_pcb(int pid);
void set_back();
#endif