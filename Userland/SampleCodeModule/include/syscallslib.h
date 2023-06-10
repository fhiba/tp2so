// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef _syscallslib_
#define _syscallslib_

#include <stddef.h>
#include <ourlib.h>

#define STDIN 1
#define STDOUT 0

int sys_read(unsigned int fd,char * buffer, size_t count);
void sys_write(unsigned int fd, int color_choice, const char * buffer, size_t count);
void sys_printMem(int lugar, unsigned char *buffer);
void sys_clear();
char sys_date(char value);
void sys_sleep(int ms);
void sys_infoRegs();
void sys_snapshotRegs(void);
void sys_resize(int num);
void sys_opcode();
void sys_divzero();
void sys_recto(int x, int y,int color, unsigned char width, unsigned char height);

void sys_printBase(uint64_t value, uint32_t base);
int sys_isBlackPixel(unsigned int x, unsigned int y);
void * sys_malloc(unsigned int mem_to_alloc);
void sys_free(void * mem_to_free);
int sys_process(uint64_t ip, uint8_t priority, uint64_t argc,char *argv[20], void *customStdin,void *customStdout);
void sys_kill(int pid);
int sys_get_pid();
void sys_block(int pid);
void sys_nice(int pid, int priority);
int sys_create_child(int ppid,uint64_t ip, uint8_t priority, uint64_t argc,char *argv[20], fd *customStdin,fd *customStdout);
void sys_wait_pid(int pid);
void sys_ps(char * buf,char * name);
my_sem sys_sem_create();
my_sem sys_sem_open(my_sem sem);
void sys_sem_post(my_sem sem);
void sys_sem_wait(my_sem sem);
void sys_create_pipe(unsigned int fds[2]);
void sys_close_pipe(unsigned int my_fd);
void sys_dup_fd(unsigned int dest_fd, unsigned int src_fd);
fd * sys_get_fd(int pid ,unsigned int searching_fd);
void * sys_memset(void * destination, int32_t c, uint64_t length);
void sys_back();
#endif