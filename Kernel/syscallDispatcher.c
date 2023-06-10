// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stddef.h>
#include <syscalls.h>
#include <scheduler.h>
#include <time.h>
#include "./include/mmu_wrapper.h"
#include <lib.h>


static int num_syscall;
extern void divzero();
extern void opcode();

void set_syscall(int num){
    num_syscall = num;
}

int sys_dispatcher(int arg0, int arg1, int arg2, int arg3,int arg4, int arg5, int arg6, int arg7){
    switch(num_syscall){
        case 99:
            set_back();
            break;
        case 1:
            read((unsigned int)arg0,(char*)(long)arg1,(size_t)arg2);
            break;
        case 2:
            write((unsigned int) arg0, arg1,(char*)(long)arg2,(size_t)arg3);
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
            return create_process((uint64_t) arg0, (uint8_t) arg1, (uint64_t) arg2,(char **) arg3, (fd *) (long) arg4,(fd *) (long) arg5);
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
        case 18:
            return (long) alloc(arg0);
            break;
        case 19:
            free((void*)(long)arg0);
            break;
        case 81:
            return (long) memset((void*)(long)arg0,(int32_t)arg1,(int64_t)arg2);
            break;
        case 20:
            printBase((uint64_t)arg0, (uint32_t) arg1);
            break;
        case 21:
            kill_process(arg0);
            break;
        case 22:
            return get_PID();
        case 23:
            isBlackPixel((unsigned int)arg0,(unsigned int) arg1);
        case 24:
            block_process(arg0);
            break;
        case 25:
            change_priority(arg0,arg1);
            break;
        case 26:
            get_process_list((char*)(long)arg0,(char *) arg1);
            break;
        case 27:
            my_sem_wait((my_sem)(long) arg0);
            break;
        case 28:
            my_sem_post((my_sem)(long) arg0);
            break;
        case 30:
            return (long) my_sem_open((my_sem)(long) arg0);
            break;
        case 29:
            create_sem();
            break;
        case 31:
            new_pipe((unsigned int *) (long)arg0);
            break;
        case 32:
            close((unsigned int) arg0);
            break;
        case 33:
            dup((unsigned int ) arg0,(unsigned int) arg1);
            break;
        case 34:
            return (long) get_fd(arg0, (unsigned int) arg1);
            break;
        case 40:
            memPrint((uint64_t) arg0, (unsigned char *) (long)arg1);
            break;
        case 35:
            return create_child(arg0,(uint64_t) arg1, (uint8_t) arg2, (uint64_t) arg3,(char **) (long) arg4, (fd *) (long)arg5,NULL);
            break;
        case 36:
            wait_pid(arg0);
            break;
        default:
            return -1;
    }
    return -1;
}