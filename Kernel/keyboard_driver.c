// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
extern unsigned char getKey();
#include <keyboard_driver.h>
#include <scheduler.h>

#define CTRL 29
#define ALT 56
int idx = 0;

unsigned char key[BUFFER_SIZE] = {0};
unsigned char last = 0;
unsigned char readKey() {
    if(idx == 0)
        return 0;
    
    
    if(last == key[idx - 1] || key[idx-1] == 0)
        return 0;
    // if(last == 0x0b10011101){
    //     saveRegisters();
    // }
    unsigned char last = key[idx-1];
    key[idx - 1] = 0;
    return last;
}

void keyboard_handler() {
    unsigned char keypressed = charHexMap[getKey()];
    key[idx % BUFFER_SIZE] = keypressed;
    if(idx == BUFFER_SIZE)
        idx = 0;
    idx++;
    if(keypressed == 18){
        int pid = get_PID();
        if(pid != 1){
            ncPrint("Killed\0");
            ncNewline();
            kill_process(pid);
        }
    }
}