#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 5
#define MAX_ARG_LENGTH 20


//typedef void (* func_type)(int argc, char argv[MAX_ARGS][MAX_ARG_LENGTH])

void parser(char * buffer);
void * get_program(char * buffer);

#endif