#ifndef SYSCALL_H_
#define SYSCALL_H_
#include <stddef.h>
#include <stdint.h>
#define STOP_PROCESS 18
#define SEND_EOF 19
void write(int fd, char* buffer, size_t count);
int read(int fd, char * buffer, size_t count);
void inforegs();
int64_t date(char value);
void printBase(uint64_t value,uint32_t base);
void memPrint(uint64_t pointer, unsigned char * buf);
#endif