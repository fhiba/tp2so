#ifndef SPEAKER_DRIVER_H_
#define SPEAKER_DRIVER_H_
#include <stdint.h>
#include <stddef.h>

void beep();
extern void outb(uint8_t, uint8_t);
extern uint8_t inb(uint8_t);
extern void _hlt(void);

#endif