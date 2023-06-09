// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <my_string.h>
void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void sleep(long ms);
char *cpuVendor(char *result);
char rtcGet(char value);
// void saveRegisters(uint64_t regsiterValues[17]);
uint64_t getDir(uint64_t dir);
uint8_t inb(uint8_t b);
void outb(uint8_t b2,uint8_t b1);
char *my_strcat(char *destination, const char *source);
void force_timer();
uint32_t cUintToBase(uint64_t value, char * buffer, uint32_t base);
#endif