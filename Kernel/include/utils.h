#ifndef UTILS_H_
#define UTILS_H_

#define BUFFER_SIZE 512


typedef struct users_node{
    int pid;
    int operation;
    struct users_node * next;
}users_node;

typedef users_node * users_list;

typedef struct pipe_struct{
    int id;
    char buffer[BUFFER_SIZE];
    uint8_t is_named;
    uint8_t is_closed;
    unsigned int read_index;
    unsigned int write_index;
    unsigned int readable_bytes;
    users_list working_processes;
    users_node * last_process_node;
    int waiting_pid;
}pipe_struct;

typedef struct pipe_struct * pipe;

typedef struct fd {
  unsigned int id;
  char readable;
  char writable;
  pipe pipe;
}fd;

#endif