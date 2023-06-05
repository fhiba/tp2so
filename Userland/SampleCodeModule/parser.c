#include <parser.h>
#include <programs.h>
#include <stddef.h>
#include <ourlib.h>
#include <syscallslib.h>

#define MAX_ARGS 5
#define MAX_ARG_LENGTH 20

int background_flag = 0;

int check_next(char * buffer, int idx){
    if(buffer[idx] == '-' && buffer[idx+1] == 'B'){
        return 1;
    }
    else if( buffer[idx] == '['){
        return 2;
    }
    return 0;
}

int catch_args(char * buffer, int idx, int * argc, char argv[MAX_ARGS][MAX_ARG_LENGTH]){
    int i = 0;
    int cant = 0;
    int next = 0;
    while(buffer[idx] != 0 && cant < MAX_ARGS && i < MAX_ARG_LENGTH){
        next = check_next(buffer,idx);
        if( next == 1 || next == 2)
                return idx;
        next = check_next(buffer,idx+1);
        if(buffer[idx] == ' ' && next == 0){
            argv[cant][i] = 0;
            cant++;
            i = 0;    
        }
        else{
            argv[cant][i++] = buffer[idx];
        }
        idx++;
    }
    argv[cant][i] = 0;
    *argc = cant;
    return idx;
}

void parse_pipe(char * buffer, char * aux1, int idx,int argc1, char argv1[MAX_ARGS][MAX_ARG_LENGTH],int flag){
    void * func1, * func2;
    char aux2[20];
    int start = 0;
    int argc2 = 0;
    char argv2[MAX_ARGS][MAX_ARG_LENGTH];
    if(flag == 0){
        my_substring(aux1,buffer,0,idx-1);
    }
    //recorro hasta encontrar el nombre del prog
    while(buffer[idx] < 'A' || buffer[idx] > 'Z'){
        idx++;
    }
    //me guardo ese index
    start = idx;
    //recorro el largo del nombre
    while(buffer[idx] >= 'A' && buffer[idx] <= 'Z'){
        idx++;
    }

    my_substring(aux2,buffer,start,idx);

    int next = check_next(buffer,idx+1);
    switch (next){
        case 0:
            catch_args(buffer,idx,&argc2,argv2);
            break;
        case 1:
            //activo background flag para prog2
        case 2:
            printerr("Can't use more than 1 pipe\n");
            return;
        default:
            break;
    }
    printf(aux1);
    printf("\n");
    for(int k = 0; k <= argc1; k++){
        printerr(argv1[k]);
        printf("\n");
    }
    printf(aux2);
    printf("\n");
    for(int k = 0; k <= argc2; k++){
        printerr(argv2[k]);
        printf("\n");
    }
    func1 = get_program(aux1);
    func2 = get_program(aux2);
    
}

void parser(char * buffer){
    void * func1;
    int len = strlen(buffer);
    int idx = 0;
    char aux[len];
    int space_flag = 0;
    int argc = 0;
    char argv[MAX_ARGS][MAX_ARG_LENGTH];
    
    while(buffer[idx] != 0 ){
        if(buffer[idx] == ' ' ){
            if(space_flag == 0)
                my_substring(aux,buffer,0,idx-1);
            space_flag = 1;
            int next = check_next(buffer, idx+1);
            switch (next)
            {
            case 0:
                idx = catch_args(buffer,idx+1,&argc,argv);
                break;
            case 1:
                background_flag = 1;
                break;
            case 2:
                parse_pipe(buffer,aux,idx,argc,argv,space_flag);
                return;
            }
        }
        else if(buffer[idx] == '-' && buffer[idx+1] == 'B'){
            background_flag = 1;
        }
        else if(buffer[idx] == '['){
            parse_pipe(buffer,aux,idx,argc,argv,space_flag);
            return;
        }
        idx++;
    }
    if(space_flag == 0)
        my_substring(aux,buffer,0,idx-1);
    
    fd * fd_aux = sys_malloc(sizeof(fd));
    func1 = get_program(aux);
    sys_process(func1,9,argc,argv,NULL,NULL,0);
}

void * get_program(char * buffer){
    for (size_t i = 0; buffer[i] != 0; i++)
    {
        if(buffer[i] == '=')
            return 0;
    }
    
    if(strcmp(buffer,"HELP"))
        return &help;
    else if(strcmp(buffer,"DATE"))
        return &date;
    else if(strcmp(buffer,"CLEAR"))
        return &clearProg;
    else if(strcmp(buffer,"RESIZE"))
        return &resize;
    else if(strcmp(buffer,"EXIT"))
        return &exitShell;
    else if(strcmp(buffer,"TRON"))
        return &tron;
    else if(strcmp(buffer,"INFOREG"))
        return &infoRegs;
    else if(strcmp(buffer, "DIVZERO"))
        return &sys_divzero;
    else if(strcmp(buffer, "OPCODE"))
        return &sys_opcode;
    else if(strcmp(buffer,"MEM"))
        return &memPrint;
    else if(strcmp(buffer,"NICE"))
        return &nice;
    else
        return 0;
    return 0;
}
