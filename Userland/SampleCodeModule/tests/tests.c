#include <tests.h>

void run_tests() {
    printf("inserte max mem para test mm:\n");
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
    if(test_mm(1,buffer) != -1)
        printf("OK!\n");
}
