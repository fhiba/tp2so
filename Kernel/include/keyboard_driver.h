// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

#define STOP_PROCESS 18
#define SEND_EOF 19
static const uint8_t charHexMap[256] = 
    {       
        0,    0,  '1',  '2',  '3',  '4',  '5',  '6',   '7',  '8',  '9',   '0',   '-',  '=',    0x7F,    '\t',
        'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',   'O',  'P',  '[',   ']',  '\n',    STOP_PROCESS,     'A',  'S',
        'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',  '\'',    0,    0,  '\\',   'Z',  'X',     'C',       'V',
        'B',  'N',  'M',  ',',  '.',  '/',    SEND_EOF,  '*',     0,  ' ',    '1',     '2',     0,    0,       0,         90,

    };

unsigned char readKey();
void keyboard_handler();

#endif