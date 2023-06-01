#ifndef PIPES_H_
#define PIPES_H_

#include <stddef.h>
#include <stdint.h>
#include <mmu_wrapper.h>
#define MAX_PIPES 64
#define READ 0
#define WRITE 1

int read_pipe(fd * user_fd, char * buffer, int max_bytes);
int write_pipe(fd * user_fd, char * buffer, int max_bytes);
uint8_t destroy_pipe(pipe * my_pipe);
int pipe_create();
uint8_t pipe_open(fd * asigned_fd, int pipe_id, int pid);
uint8_t pipe_close(pipe * my_pipe, int pid);

#endif