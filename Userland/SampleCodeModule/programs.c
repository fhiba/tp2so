#include <programs.h>
#include <ourlib.h>
#include <syscallslib.h>
#include <tron.h>

void exit(){
    int pid = sys_get_pid();
    sys_kill(pid);
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
    printf(": Filters the vocals frome the input\n");
    printFirst("PHYLO");
    printf(": Runs phylo\n");
    printFirst("CLEAR");
    printf(": Clean the screen\n");
    exit();
    return;
}

void nice(int argc,char argv[5][20]){
    if(argc != 2){
        printerr("Wrong amount of arguments\n");
        exit();
    }
    printf("Changed the priority of ");
    printf(argv[0]);
    printf(" to ");
    printf(argv[1]);
    printf("\n");
    int aux1 = atoi(argv[0]);
    int aux2 = atoi(argv[1]);
    sys_nice(argv[0],argv[1]);
    exit();
}

void ps(int argc,char argv[5][20]){
    printf("program list jaja\n");
    sys_ps();
    exit();
}

void loop(int argc,char argv[5][20]){
    for(int i = 0; i < 5;i++){
        printf("PID:2 HOLA!\n");
        sys_sleep(2000);
    }
    printf("jaja pensaste que funcionaba lrpm sos re boludo\n");
    exit();
}

void kill(int argc, char argv[5][20]){
    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    //falta pasar de string a int
    sys_kill(argv[0]);
    exit();
}

void block(int argc,char argv[5][20]){
    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    sys_block(argv[0]);
    exit();
}

void cat(int argc,char argv[5][20]){
    printf("meow\n");
    exit();
    //imprime el stdin tal como lo recibe
}

void wc(int argc,char argv[5][20]){
    printf("water closet\n");
    exit();
    //cuenta la cantidad de lineas del input
}

int is_vow(char letter){
    if(letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U')
        return 1;
    return 0;
}

void filter_vow(int argc, char argv[5][20]){
    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    int i, j;
    for(i = 0, j = 0; argv[0][i] != 0;i++){
        if(!is_vow(argv[0][i]))
            argv[0][j++] = argv[0][i];
    }
    argv[0][j] = 0;
    printf(argv[0]);
    printf("\n");
    exit();
}

void phylo(int argc,char argv[5][20]){
    printf("deja de sacarle filo a la espada que te vas a quedar ciego pedro\n");
    //corre el programa de los filosofos
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

    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        int pid = sys_get_pid();
        sys_kill(pid);
    }
    int aux = atoi(argv[0]);
    sys_resize(aux);
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
    sys_create_child(sys_get_pid(),&sleep,9,NULL,NULL,NULL,NULL,0);
    sys_create_child(sys_get_pid(),&sleep,9,NULL,NULL,NULL,NULL,0);
    sys_wait_pid(sys_get_pid());
    printf("termina proceso padre\n");
    sys_kill(sys_get_pid());
}


void memPrint(int argc, char argv[5][20]){
    if(argc != 1){
        printerr("Wrong amount of arguments\n");
        return;
    } 
    int ok = 1;
    uint64_t dir = stringToUint64(argv[0],&ok);
    if(!ok || strlen(argv[0]) > 9){
        printerr("Invalid adress\n");
        return;
    }
    unsigned char memStr[20] = {0};
    sys_printMem(dir,memStr);
    for(int i=0; i < 20; i++){
        if(i != 0 && i %5 ==0)
            putchar('\n');
        if(memStr[i] <16){
            putchar('0');
        }
        sys_printBase(memStr[i],16);

        putchar(' ');
    }
    putchar('\n');
    exit();
}
