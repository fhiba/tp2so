#include <programs.h>
#include <ourlib.h>
#include <syscallslib.h>
#include <tron.h>

void help(){
    printFirst("Available Commands: \n");
    printFirst("TRON");
    printf(": Play a two player game with a friend\n");
    printFirst("DATE");
    printf(": Show the current date and time\n");
    printFirst("RESIZE");
    printf(": Change the size of the font\n");
    printFirst("DIVZERO");
    printf(": See what the divzero exception looks like\n");
    printFirst("OPCODE");
    printf(": See what the opcode exception loooks like\n");
    printFirst("INFOREG");
    printf(": After taking a snapshot with = see what values the registers hold!\n");
    printFirst("MEM");
    printf(": Dumps 20 bytes of memory from given pointer\n");
    printFirst("NICE");
    printf(": Changes the priority of a process to the given priority");
    printFirst("CLEAR");
    printf(": Clean the screen\n");
}

void nice(){
    printf("Insert the PID\n");    
    char buffer1[90] = {0};
    int idx = 0;
    do{
            sys_read(1, buffer1 + idx, 1);
            if(buffer1[idx] == 0){
                
            }
            else if(buffer1[idx] != 0x7F){
                sys_write(1, buffer1 + idx, 1);
                idx++;
            }
            else if (idx > 0) {
                sys_write(1, buffer1 + idx, 1);
                idx--;
            }
    }while(buffer1[idx-1] != '\n');
    buffer1[idx-1] = 0;

    printf("Choose the new priority\n");    
    char buffer2[90] = {0};
    int idx = 0;
    do{
            sys_read(1, buffer2 + idx, 1);
            if(buffer2[idx] == 0){
                
            }
            else if(buffer2[idx] != 0x7F){
                sys_write(1, buffer2 + idx, 1);
                idx++;
            }
            else if (idx > 0) {
                sys_write(1, buffer2 + idx, 1);
                idx--;
            }
    }while(buffer2[idx-1] != '\n');
    buffer2[idx-1] = 0;

}

void clearProg(){
    clear();
}

void resize(){
    printf("Choose a multiplier to resize the font!\n");
    char buffer[2] = {0};

    do{
        sys_read(1, buffer, 1);
    }while(buffer[0] == 0);

    sys_write(1, buffer, 1);
    printf("\n");
    int aux = atoi(buffer);
    sys_resize(aux);
}

void exitShell(){
    printf("exit?\n");
}


char getDateValues(char value){
    return sys_date(value);
}

void date(){
    int values[5] = {7,8,9,4,2};
    for (int i=0;i<5;i++){
        char num = getDateValues(values[i]);
        sys_printBase(num, 16);
        if (i<2){
            printf("/");
        } else if (i==3){
            printf(":");
        } else {
            printf(" ");
        }
    }
    printf("UTC\n");
}

void tron(){
    clear();
    printf("Welcome to tron!\nThe game will start soon! \nPlayer 1 moves with WASD and is color blue\nPlayer 2 moves IJKL and is color red\nGOOD LUCK, HAVE FUN!\n");
    sys_sleep(2500);
    tronRun();
}

void infoRegs(){
     sys_infoRegs();
}


void memPrint(){
    printf("Enter a hexadecimal address of 8 characters at max\n");
    printf("Example: 0000000F\n");
    char buffer[90] = {0};
    int idx = 0;
    do{
            sys_read(1, buffer + idx, 1);
            if(buffer[idx] == 0){
                
            }
            else if(buffer[idx] != 0x7F){
                sys_write(1, buffer + idx, 1);
                idx++;
            }
            else if (idx > 0) {
                sys_write(1, buffer + idx, 1);
                idx--;
            }
    }while(buffer[idx-1] != '\n');
    buffer[idx-1] = 0;    
    int ok = 1;
    uint64_t dir = stringToUint64(buffer,&ok);
    if(!ok || idx > 9){
        printerr("Invalid adress\n");
        return;
    }
    unsigned char memStr[20] = {0};
    sys_printMem(dir,memStr);
    for(int i=0; i < 20; i++){
        if(i != 0 && i %5 ==0)
            putchar('\n');
        if(memStr[i] <16){
            putchar('0');
        }
        sys_printBase(memStr[i],16);

        putchar(' ');
    }
    putchar('\n');
}
