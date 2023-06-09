// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SYSDISPATCHER_H_
#define SYSDISPATCHER_H_
void set_syscall(int num);
int sys_dispatcher(int arg0, int arg1, int arg2,int arg3,int arg4,int arg5, int arg6, int arg7);
#endif
