// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <programs.h>
#include <syscallslib.h>
#include <shell.h>
#include <tests.h>
#include <tron.h>
#include <parser.h>

#define CTRL 18
void shell(void){
    char buffer[100] = {0};
    int idx = 0;
    while(1){
        printFirst("$ ");
        do{
            sys_read(STDIN, buffer + idx, 1);
            if(buffer[idx] == 0){
                
            }
            else if(buffer[idx] != 0x7F){
                if(buffer[idx] == '='){
                    printFirst("\nSnapshot Tomado\n");
                    break;
                }else if(buffer[idx] == CTRL){
                    continue;
                }else{   
                    sys_write(STDOUT,1, buffer + idx, 1);
                }
                idx++;
            }
            else if (idx > 0) {
                sys_write(STDOUT,1, buffer + idx, 1);
                idx--;
            }
        }while(buffer[idx-1] != '\n');
        buffer[idx-1] = 0;         
        parser(buffer);
        idx = 0;
    }
}
