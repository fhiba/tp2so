// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <programs.h>
#include <ourlib.h>
#include <syscallslib.h>
#include <tron.h>

void exit(){
    int pid = sys_get_pid();
    sys_kill(pid);
}

void test_sems(){
    my_sem semaphore = sys_sem_create();
    sys_sem_wait(semaphore);
    printf("entre\n");
    sys_sem_post(semaphore);
    exit();
}

void help(){
    printFirst("Available Commands: \n");
    printFirst("TRON");
    printf(": Play a two player game with a friend\n");
    printFirst("DATE");
    printf(": Show the current date and time\n");
    printFirst("RESIZE");
    printf(": Change the size of the font\n");
    printFirst("DIVZERO");
    printf(": See what the divzero exception looks like\n");
    printFirst("OPCODE");
    printf(": See what the opcode exception loooks like\n");
    printFirst("INFOREG");
    printf(": After taking a snapshot with = see what values the registers hold!\n");
    printFirst("MEM");
    printf(": Dumps 20 bytes of memory from given pointer\n");
    printFirst("NICE");
    printf(": Changes the priority of a process to the given priority\n");
    printFirst("PS");
    printf(": Prints a list of all the processes and their properties\n");
    printFirst("LOOP");
    printf(": Prints the ID and a message every 3 seconds\n");
    printFirst("KILL");
    printf(": Kills the process of a certain ID\n");
    printFirst("BLOCK");
    printf(": Changes the state of a process between blocked and ready given its ID\n");
    printFirst("CAT");
    printf(": Prints the contents of the standard input\n");
    printFirst("WC");
    printf(": Counts the amount of input lines\n");
    printFirst("FILTER");
    printf(": Filters the vocals from the input\n");
    printFirst("PHYLO");
    printf(": Runs phylo\n");
    printFirst("MEM");
    printf(": Prints the state of the memory\n");
    printFirst("CLEAR");
    printf(": Clean the screen\n");
    printFirst("TESTMM");
    printf(": Periodically allocates and free mem up to max mem given as argument\n");
    printFirst("TESTPRIO");
    printf(": Creates up to MAX_PROCESSES and changes it's priority in scheduler\n");
    printFirst("TESTPROCESSES");
    printf(": Creates up to MAX_PROCESSES and randomly changes it's state to (UN)BLOCK or KILLs them\n");
    printFirst("TESTSYNC");
    printf(": Tests sync between processes changing global variable value, receives max modifications to var per process\n");
    exit();
}

void invalid(){
    printerr("INVALID COMMAND\n");
    exit();
}

int parse_buffer(char * s, char * a, char * b) {
    int buff_picker = 0;
    while(*s != '\0') {
        if(*s != ' ' && (*s < '0' || *s >'9'))
            return 1;
        if(*s == ' ') {
            if(!buff_picker) {
                buff_picker++;
                *a = '\0';
            } else
                return 1;
        } else {
            if(!buff_picker) {
                *a = *s;
                a++;
            } else {
                *b = *s;
                b++;
            }
        }
        s++;
    }
    *b = '\0';
    return 0;
}

void nice(int argc,char argv[5][20]){
    char buffer[512];
    char s_aux1[20];
    char s_aux2[20];
    int err_flag = 1;
    int flag_pipe = 0;
    if(argc != 3){
        if(sys_check_pipe(STDIN) == 0){
            printerr("wrong amount of arguments\n");
            exit();
        }
        flag_pipe = 1;
        int acum = 0;
        int bytes_read = 0;
        while((bytes_read = sys_read(STDIN, buffer + acum, 512)) != -1)
            acum+= bytes_read;

        buffer[acum - 1] = 0;
        err_flag = parse_buffer(buffer, s_aux1, s_aux2);
        flag_pipe = 0;
        if(err_flag != 0){
            printerr("wrong amount of arguments\n");
            exit();
        }

    }
    printf("Changed the priority of ");
    printf(flag_pipe? s_aux1:argv[1]);
    printf(" to ");
    printf(flag_pipe? s_aux2:argv[2]);
    printf("\n");
    int aux1 = atoi(flag_pipe? s_aux1:argv[1]);
    int aux2 = atoi(flag_pipe? s_aux2:argv[2]);
    sys_nice(aux1,aux2);
    exit();
}

void ps(int argc,char argv[5][20]){
    char buffer[1024] = {0};
    sys_ps(buffer,argv[0]);
    printf(buffer);
    exit();
}

void loop(int argc,char argv[5][20]){
    int pid = sys_get_pid();
    char num[20];
    intToString(pid,num);
    while(1){
        printf(num);
        printf("\n");
        printf("HOLA!");
        printf("\n");
        sys_sleep(500);
    }
    exit();
}

void kill(int argc, char argv[5][20]){
    char buffer[20] = {0};
    int flag_pipe = 0;
    if(argc != 2){
        if(sys_check_pipe(STDIN) == 0){
            printerr("wrong amount of arguments\n");
            exit();
        }
        int bytes_read;
        while((bytes_read = sys_read(STDIN, buffer, 20)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = 0;
                flag_pipe = 1;
            }
        }
    }
    int num = atoi(flag_pipe?buffer:argv[1]);
    sys_kill(num);
    exit();
}

void block(int argc,char argv[5][20]){
    char buffer[100];
    int flag_pipe = 0;
    if(argc != 2){
        if(sys_check_pipe(STDIN) == 0){
            printerr("wrong amount of arguments\n");
            exit();
        }
        int bytes_read;
        while((bytes_read = sys_read(STDIN, buffer, 100)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = 0;
                flag_pipe = 1;
            }
        }
    }
    int num;
    if(flag_pipe)
        num = atoi(buffer);
    else
        num = atoi(argv[1]);
    int flag = sys_block(num);
    if(flag == -1)
        printerr("There was a problem blocking the process\n");
    printf(flag_pipe?argv[1]:buffer);
    printf(" : ");
    if(flag == 1){
        printf("BLOCKED\n");
    }
    else if(flag == 0){
        printf("READY\n");
    }
    exit();
}

void cat(int argc,char argv[5][20]){
    if(argc != 2) {
        if(sys_check_pipe(STDIN) == 0){
            printerr("wrong amount of arguments\n");
            exit();
        }
        char buffer[100];
        int bytes_read;
        while((bytes_read = sys_read(STDIN, buffer, 100)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = 0;
                printf(buffer);
            }
        }
        printf("\n");
        exit();
    }

    for(int i = 1; i < argc; i++){
            printf(argv[i]);
            printf("\n");
    }
    exit();
}

void wc(int argc,char argv[5][20]){
    if(argc != 2) {
        printf("entre\n");
        char buffer[512] = {0};
        int bytes_read;
        int count = 0;
        while((bytes_read = sys_read(STDIN, buffer, 512)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = 0;
                for(int i = 0; i < bytes_read -1; i++){
                    if(buffer[i] == '\n'){
                        count++;
                        printf("+1");
                    }
                }
            }
        }
        printf("sali");
        char num[5];
        intToString(count,num);
        printf(num);
        printf("\n");
        exit();
    }

    int count = -1 + argc;
    for(int i = 1; i < argc; i++){
        for(int j = 0; j < strlen(argv[i]);j++){
            if(argv[i][j] == '\n')
                count++;
        }
    }
    char num[10];
    intToString(count,num);
    printf(num);
    printf("\n");
    exit();
}

int is_vow(char letter){
    if(letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U' ||
        letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
        return 1;
    return 0;
}

void filter_vow(int argc, char argv[5][20]){
    char buffer[512] = {0};
    int bytes_read = 0;
    if(argc != 2){
        if(sys_check_pipe(STDIN) == 0){
            printerr("wrong amount of arguments\n");
            exit();
        }
        while((bytes_read = sys_read(STDIN, buffer, 512)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = '\0';
                for(int i = 0; i < bytes_read; i++) {
                    if(!is_vow(buffer[i]))
                        putchar(buffer[i]);
                }
            }
        }
        printf("\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    int i, j;
    for(i = 0, j = 0; argv[1][i] != 0;i++){
        if(!is_vow(argv[1][i]))
            argv[1][j++] = argv[1][i];
    }
    argv[1][j] = 0;
    printf(argv[1]);
    printf("\n");
    exit();
}

void run_phylo(int argc,char argv[5][20]){
    phylo();
    exit();
}

void clearProg(){
    clear();
    exit();
}

void resize(int argc,char argv[5][20]){
    // printf("Choose a multiplier to resize the font!\n");
    // char buffer[2] = {0};

    // do{
    //     sys_read(1, buffer, 1);
    // }while(buffer[0] == 0);

    // sys_write(1, buffer, 1);
    // printf("\n");
    char buffer[512] = {0};
    int bytes_read = 0;
    if(argc != 2){
        while((bytes_read = sys_read(STDIN, buffer, 512)) != -1) {
            if(bytes_read != 0) {
                buffer[bytes_read - 1] = '\0';
                for(int i = 0; i < bytes_read; i++) {
                    if(!is_vow(buffer[i]))
                        putchar(buffer[i]);
                }
            }
        }
        printf("\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    if(argc != 2){
        printerr("Wrong amount of arguments\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    int num = atoi(argv[1]);
    sys_resize(num);
    exit();
}

void exitShell(){
    printf("exit?\n");
    exit();
}


char getDateValues(char value){
    return sys_date(value);
}

void date(){
    int values[5] = {7,8,9,4,2};
    for (int i=0;i<5;i++){
        char num = getDateValues(values[i]);
        sys_printBase(num, 16);
        if (i<2){
            printf("/");
        } else if (i==3){
            printf(":");
        } else {
            printf(" ");
        }
    }
    printf("UTC\n");
    exit();
}

void tron(){
    clear();
    printf("Welcome to tron!\nThe game will start soon! \nPlayer 1 moves with WASD and is color blue\nPlayer 2 moves IJKL and is color red\nGOOD LUCK, HAVE FUN!\n");
    sys_sleep(2500);
    tronRun();
}

void infoRegs(){
    sys_infoRegs();
    exit();
}

void sleep(){
    printf("empieza proceso hijo\n");
    sys_sleep(2000);
    printf("termina proceso hijo\n");
    sys_kill(sys_get_pid());
}

void test_childs(){
    printf("entra al proceso padre\n");
    sys_create_child(sys_get_pid(),(uint64_t)&sleep,9,(uint64_t)NULL,NULL,NULL,NULL);
    sys_create_child(sys_get_pid(),(uint64_t)&sleep,9,(uint64_t)NULL,NULL,NULL,NULL);
    sys_wait_pid(sys_get_pid());
    printf("termina proceso padre\n");
    sys_kill(sys_get_pid());
}


void mem(int argc, char argv[5][20]){
    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        return;
    } 
    // int ok = 1;
    // uint64_t dir = stringToUint64(argv[1],&ok);
    // if(!ok || strlen(argv[0]) > 9){
    //     printerr("Invalid adress\n");
    //     return;
    // }
    // unsigned char memStr[20] = {0};
    // sys_printMem(dir,memStr);
    // for(int i=0; i < 20; i++){
    //     if(i != 0 && i %5 ==0)
    //         putchar('\n');
    //     if(memStr[i] <16){
    //         putchar('0');
    //     }
    //     sys_printBase(memStr[i],16);

    //     putchar(' ');
    // }
    unsigned int status[3];
    char aux[50];
    sys_mem_status(status);

    intToString(status[0],aux);
    printFirst("Heap Size : ");
    printf(aux);
    putchar('\n');
    intToString(status[1],aux);
    printFirst("Heap Left : ");
    printf(aux);
    putchar('\n');
    intToString(status[2],aux);
    printFirst("Used Heap : ");
    printf(aux);
    putchar('\n');
    exit();
}
