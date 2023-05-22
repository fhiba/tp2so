#include <syscalls.h>
#include "./include/naiveConsole.h"
#include <interrupts.h>
#include <lib.h>
#include <mmu.h>

#define STDIN 1
#define RETVALUE -1

static char snapshot = 0;
static const char* registerNames[17] = {
    "RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"
};

extern uint64_t registers[17];
extern char registersSaved;

void write(int fd, char*buffer, size_t count){
    int color = 0;
    for(int i =0;i<count;i++){
        if(buffer[i] == '\n'){
            ncNewline();
        } else if (buffer[i] == 0x7F) {
            deleteChar();
        } else {
            switch(fd){
            case 1:
                color = BLANCO;
                break;
            case 2:
                color = ROJO;
                break;
            case 3:
                color = VERDE;
                break;
            case 4:
                color = CELESTE;
                break;
            }
            drawChar(buffer[i],color,NEGRO);
        }
    }
}

int read(int fd, char * buffer, size_t count){
    if(fd == STDIN)
    {
        int k = 0;
        unsigned char key = 0;
        while(k < count ){
            _hlt();
            key = readKey();
            buffer[k++] = key;
        }
        return k;
    }
    return -1;
}

void clear(){
    ncClear();
}

void resize(int num){
    nResize(num);
}

char mydate(char value){
    return rtcGet(value);
}



void inforegs(){
    if(!registersSaved){
        write(2,"Primero debe tomar un snapshot con la tecla ctrl",49);
        ncNewline();
    }else{
        for(int i = 0;i<17;i++){
            write(4,registerNames[i],3);
            write(1,": ",2);
            ncPrintBase(registers[i],16);
            ncNewline();
        }
    }
}

uint64_t xtou64(const char *str)
{
    uint64_t res = 0;
    char c;

    while ((c = *str++)) {
        char v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8);
        res = (res << 4) | (uint64_t) v;
    }

    return res;
} 

void printMem(uint64_t pointer, unsigned char * buf)
{
	uint8_t *start = (uint8_t *)pointer;
	for (int i = 0; i < 32; i++)
	{
		buf[i] = start[i];
	}
}

void printBase(uint64_t value, uint32_t base) {
    ncPrintBase(value, base);
}

void memPrint(uint64_t pointer, unsigned char * buf)
{
	uint8_t *start = (uint8_t *)pointer;
	for (int i = 0; i < 20; i++)
	{
		buf[i] = start[i];
	}
}