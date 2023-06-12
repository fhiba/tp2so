// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef PIPES_H_
#define PIPES_H_

#include <my_string.h>
#include <stddef.h>
#include <stdint.h>
#include <mmu_wrapper.h>
#include <lib.h>
#include <utils.h>
#define READ 0
#define WRITE 1





int read_pipe(struct fd * user_fd, char * buffer, int max_bytes);
int write_pipe(struct fd * user_fd, char * buffer, int max_bytes);
uint8_t destroy_pipe(pipe my_pipe);
int pipe_create(int is_named);
uint8_t pipe_open(struct fd * asigned_fd, int pipe_id, int pid);
uint8_t pipe_close(pipe my_pipe, int pid);
pipe get_pipe(int pipe_id);
int named_pipe_create(char * name);
uint8_t destroy_named_pipe(char * name);
uint8_t open_named_pipe(struct fd * asigned_fd, char * name , int pid);
int get_pipe_id(pipe my_pipe);
#endif