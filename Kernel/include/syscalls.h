#ifndef SYSCALL_H_
#define SYSCALL_H_
#include <stddef.h>
#include <interrupts.h>
#include <lib.h>
#include <mmu.h>
#include <stdint.h>
#include <scheduler.h>
#include <naiveConsole.h>
#include <keyboard_driver.h>
#define READ 0
#define WRITE 1

void write(unsigned int my_fd, int color_choice, char*buffer, size_t count);
int read(unsigned int my_fd, char * buffer, size_t count);
void inforegs();
int64_t date(char value);
void printBase(uint64_t value,uint32_t base);
void memPrint(uint64_t pointer, unsigned char * buf);
void new_pipe(unsigned int fds[2]);
uint8_t create_named_pipe(char * name);
uint8_t named_pipe_destroy(char * name);
int named_pipe_open(char * name, uint8_t operation);
void close(unsigned int my_fd);
void dup(unsigned int dest_fd, unsigned int src_fd);
#endif