// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 5
#define MAX_ARG_LENGTH 20


//typedef void (* func_type)(int argc, char argv[MAX_ARGS][MAX_ARG_LENGTH])

void parser(char * buffer);
void * get_program(char * buffer);

#endif