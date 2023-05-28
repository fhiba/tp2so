#ifndef _syscallslib_
#define _syscallslib_

#include <stddef.h>
#include <ourlib.h>

int sys_read(int fd,char * buffer, size_t count);
void sys_write(int fd, const char * buffer, size_t count);
void sys_printMem(int lugar, unsigned char *buffer);
void sys_clear();
char sys_date(char value);
void sys_sleep(int ms);
void sys_setScreen(int mode);
void sys_infoRegs();
void sys_snapshotRegs(void);
void sys_resize(int num);
void sys_opcode();
void sys_divzero();
void sys_recto(int x, int y,int color, unsigned char width, unsigned char height);
void sys_beep();
void sys_printBase(uint64_t value, uint32_t base);
int sys_isBlackPixel(unsigned int x, unsigned int y);
void * sys_malloc(unsigned int mem_to_alloc);
void sys_free(void * mem_to_free);
void * sys_memset(void * destination, int32_t c, uint64_t length);
#endif