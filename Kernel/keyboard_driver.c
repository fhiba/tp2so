extern unsigned char getKey();
#include <keyboard_driver.h>

#define BUFFER_SIZE 100

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
    key[idx % BUFFER_SIZE] = charHexMap[getKey()];
    if(idx == BUFFER_SIZE)
        idx = 0;
    idx++;
}