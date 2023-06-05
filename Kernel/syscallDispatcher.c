#include <stddef.h>
#include <syscalls.h>
#include <speakerDriver.h>
#include <scheduler.h>
#include "./include/time.h"
#include "./include/mmu_wrapper.h"


static int num_syscall;
extern void divzero();
extern void opcode();

void set_syscall(int num){
    num_syscall = num;
}

int sys_dispatcher(int arg0, int arg1, int arg2, int arg3,int arg4, int arg5, int arg6){
    switch(num_syscall){
        case 1:
            read(arg0,(char*)arg1,(size_t)arg2);
            break;
        case 2:
            write(arg0,(char*)arg1,(size_t)arg2);
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
        case 6:
            return mydate(arg0);
            break;
        case 7:
            tsleep((long)arg0);
            break;
        case 8:
            create_process((uint64_t) arg0, (uint8_t) arg1, (uint64_t) arg2,(char **) arg3, (fd *)arg4,(fd *)arg5, (uint8_t) arg6);
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
            return alloc(arg0);
            break;
        case 19:
            free(arg0);
            break;
        case 20:
            printBase((uint64_t)arg0, (uint32_t) arg1);
            break;
        case 21:
            kill_process(arg1);
            break;
        case 22:
            return get_PID();
        case 23:
            isBlackPixel((unsigned int)arg0,(unsigned int) arg1);
        case 24:
            block_process(arg1);
            break;
        case 25:
            change_priority(arg1,arg2);
            break;
        case 26:
            get_process_list();
            break;
        case 40:
            memPrint((uint64_t) arg0, (unsigned char *) arg1);

        default:
            return -1;
    }
    return -1;
}