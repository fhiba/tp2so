#include <stddef.h>
#include <syscalls.h>
#include <speakerDriver.h>
#include <time.h>
#include "./include/mmu_wrapper.h"
#include <lib.h>


static int num_syscall;
extern void divzero();
extern void opcode();

void set_syscall(int num){
    num_syscall = num;
}

int sys_dispatcher(int arg0, int arg1, int arg2, int arg3,int arg4){
    switch(num_syscall){
        case 1:
            read(arg0,(char*) (long) arg1,(size_t)arg2);
            break;
        case 2:
            write(arg0,(char*) (long)arg1,(size_t)arg2);
            break;
        case 3:
            clear();
            break;
        case 4:
            // takeSnapshot();
            break;
        case 5:
            inforegs();
            break;
        case 7:
            tsleep((long)arg0);
            break;
        case 6:
            return mydate(arg0);
            break;
        case 20:
            printBase((uint64_t)arg0, (uint32_t) arg1);
            break;
        case 9:
            resize(arg0);
            break;
        case 14:
            divzero();
            break;
        case 15:
            opcode();
            break;
        case 16:
            fillRect(arg0,arg1,arg2,arg3,arg4);
            break;
        case 17:
            beep();
            break;
        case 18:
            return (long) alloc(arg0);
            break;
        case 19:
            free((void*)(long)arg0);
            break;
        case 21:
            return memset(arg0,arg1,arg2);
            break;
        case 40:
            memPrint((uint64_t) arg0, (unsigned char *) (long) arg1);
        case 23:
            isBlackPixel((unsigned int)arg0,(unsigned int) arg1);
        default:
            return -1;
    }
    return -1;
}