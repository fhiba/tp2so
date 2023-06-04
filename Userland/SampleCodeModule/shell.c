#include <programs.h>
#include <syscallslib.h>
#include <shell.h>
#include <tron.h>
#include <parser.h>


void shell(void){
    char buffer[100] = {0};
    int idx = 0;
    while(1){
        printFirst("$ ");
        do{
            sys_read(1, buffer + idx, 1);
            if(buffer[idx] == 0){
                
            }
            else if(buffer[idx] != 0x7F){
                if(buffer[idx] == '='){
                    printFirst("\nSnapshot Tomado\n");
                    break;
                }else{   
                    sys_write(1, buffer + idx, 1);
                }
                idx++;
            }
            else if (idx > 0) {
                sys_write(1, buffer + idx, 1);
                idx--;
            }
        }while(buffer[idx-1] != '\n');
        buffer[idx-1] = 0;         
        parser(buffer);
        idx = 0;
    }
}
