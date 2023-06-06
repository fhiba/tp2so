#include <scheduler.h>


#define DEFAULT_PROG_MEM 4096




#define NAME 0
#define ID 1
#define PRIORITY 2
#define RSP 3
#define RBP 4
#define FOREGROUND 5
#define STATE 6


typedef struct process_node {
    pcb *pcb;
    struct process_node *next;
} process_node;

typedef struct Scheduler {
    process_node *process_list;
    process_node *current;
} Scheduler;


Scheduler *scheduler;
int currentPID = 1;
process_node *starting_process;
pcb *startingPCB;
uint8_t firstProcess = 1;
uint8_t back = 0;

#define BLOCKED 0
#define READY 1
#define RUNNING 2
#define EXITED 3

void insert_by_priority(process_node ** head, process_node* newNode);

/* Function to bubble sort the given linked list */
void bubbleSort(process_node *start); 
  
/* Function to swap data of two nodes a and b*/
void swap(process_node *a, process_node *b);


void normalizeSpaces(char *buf, char *data, int field);
void free_fd(fd* fd_to_free,int process_id);
void insert_in_pid_list(int ppid,int pid);
fdNode * get_fd_node(fdNode * list, unsigned int fd_id); 
process_node* get_process_node(int process_id);
fdNode * remove_fd_from_list(fdNode* list, unsigned int fd, int pid);
void free_fd_list(fdNode* fdNode_to_free, int process_id);
void free_child_pid_list(pid_node* child_list_to_free);


pcb * get_pcb(int pid){
    process_node * aux = get_process_node(pid);
    return aux->pcb;
}


process_node* get_process_node(int process_id);

static void default_process() {
  while (1) {
    _hlt();
  }
}

//falta ultimo = null??
void initScheduler(){
    scheduler = (Scheduler *)alloc(sizeof(Scheduler));
    scheduler->process_list = NULL;
    uint64_t defaultProcessMem = (uint64_t)alloc(2048);

    uint64_t sp = initProcess(defaultProcessMem + 2048,
                                (uint64_t)&default_process, 0, NULL);

    startingPCB = (pcb *)alloc(sizeof(pcb));
    startingPCB->pid = 0;
    startingPCB->stackPointer = sp;
    startingPCB->processMemory = defaultProcessMem;
    startingPCB->state = 1; 
    startingPCB->priority = 1;  
    startingPCB->auxPriority = 1;
    starting_process = (process_node *)alloc(sizeof(process_node));
    starting_process->pcb = startingPCB;
    starting_process->next = scheduler->process_list;
}

int strlen(char * string){
    int i = 0;
    while(string[i]!=0){
        i++;
    }
    return i;
}

int create_process(uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout){
    //CREO EL PROCESO
    pcb *newPCB = (pcb *)alloc(sizeof(pcb));
    if(newPCB == NULL)
        return -1;
    newPCB->pid = currentPID++;
    newPCB->priority = priority;
    newPCB->auxPriority = priority;
    newPCB->fds = NULL;
    newPCB->last_node = NULL;

    if(customStdin == NULL) {
        customStdin = alloc(sizeof(fd));
        customStdin->id = STDIN;
        customStdin->pipe = NULL;
        customStdin->readable = 1;
        customStdin->writable = 0;
    }

    if(customStdout == NULL) {
        customStdout = alloc(sizeof(fd));
        customStdout->id = STDOUT;
        customStdout->pipe = NULL;
        customStdout->readable = 0;
        customStdout->writable = 1;
    }

    newPCB->stdin_fd = customStdin;
    newPCB->stdin_fd->id = 0;
    newPCB->stdout_fd = customStdout;
    newPCB->stdout_fd->id = 1;
    newPCB->next_fd_id = 2;
    newPCB->state = 1;
    newPCB->background = back;
    back = 0;

    
    for (int i = 0; i < argc; i++) {
        for (int j = 0; j < strlen(argv[i]); j++) {
            newPCB->args[i][j] = argv[i][j];
        }
    }
    uint64_t processMemory = (uint64_t)alloc(DEFAULT_PROG_MEM);
    if((void *)processMemory == NULL) {
        free(newPCB);
        return -1;
    }

    uint64_t sp = initProcess(processMemory + DEFAULT_PROG_MEM, ip, argc, newPCB->args);
    newPCB->stackPointer = sp;
    newPCB->basePointer = processMemory + DEFAULT_PROG_MEM - 1;  // no se si aca falta un -1
    newPCB->processMemory = processMemory;
    pid_node * new_child_pid_list = NULL;
    process_node *newNode = (process_node *)alloc(sizeof(process_node));
    if(newNode == NULL) {
        free(newPCB);
        free((void*)processMemory);
        return -1;
    }
    newNode->pcb = newPCB;
    insert_by_priority(&(scheduler->process_list),newNode);
    force_timer();
    return newPCB->pid;
} 

void set_back(){
    back = 1;
}

int create_child(int ppid,uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout){

    int child_pid = create_process(ip,priority,argc,argv,customStdin,customStdout);
    
    process_node* parent_node = get_process_node(child_pid);

    insert_in_pid_list(ppid,child_pid);
    return child_pid;
}

void insert_in_pid_list(int ppid,int pid){
    process_node *parent_node = get_process_node(ppid);
    pid_node *aux_pid_node = parent_node->pcb->child_pid_list;
    pid_node* new_pid_node = (pid_node*) alloc(sizeof(pid_node));
    new_pid_node->next = NULL;
    new_pid_node->pid = pid;
    if(aux_pid_node == NULL){
        parent_node->pcb->child_pid_list = new_pid_node;
    }else{
        while(aux_pid_node->next != NULL)
            aux_pid_node = aux_pid_node->next;
        aux_pid_node->next = new_pid_node;
    }
}



void insert_by_priority(process_node ** head, process_node* newNode)
{
    // special case for the head end
    if (*head == NULL || (*head)->pcb->priority <= newNode->pcb->priority) {
        newNode->next = *head;
        *head = newNode;
        return;
    }
 
    // locate the node before the point of insertion
    process_node* current = *head;
    while (current->next != NULL && current->next->pcb->priority > newNode->pcb->priority) {
        current = current->next;
    }
 
    newNode->next = current->next;
    current->next = newNode;
}

void shell_handler(process_node * process){
    if(process->pcb->pid == 1){ //es la shell?
        process_node * iter = scheduler->process_list;
        int flag = 0;
        if(process->pcb->state == BLOCKED){
            while(iter != NULL){
                if(iter->pcb->pid != 1 && iter->pcb->state == READY && iter->pcb->background == 0)
                    flag = 1;
                iter = iter->next;
            }
            if(flag == 0){
                process->pcb->state = READY;
            }
        }
        else{
            while(iter != NULL){
                if(iter->pcb->pid != 1 && iter->pcb->state != BLOCKED){
                    flag = 1;
                }
                iter = iter->next;
            }
            if(flag){
                process->pcb->state = BLOCKED;
            }
        }
    }
}

int switch_context(int rsp){

    if (firstProcess) {
        firstProcess = 0;
        scheduler->current = scheduler->process_list;

        //SI ESTA BLOQUEADO EL PRIMER PROCESO CORRO EL DEFAULT
        if(scheduler->current->pcb->state == BLOCKED)
            return starting_process->pcb->stackPointer;
        
        scheduler->current->pcb->auxPriority--;
        return scheduler->current->pcb->stackPointer;
    }
    
    //ME GUARDO EL STACK POINTER DEL PROCESO QUE ESTABA CORRIENDO 
    scheduler->current->pcb->stackPointer = rsp;
    
    

    //ELIGO QUIEN VA
    process_node *aux_node = scheduler->process_list;
    process_node *prev_aux_node = aux_node;
    //ESTE FLAG LO USO PARA SABER SI ESTAN TODOS LOS PROCESOS BLOQUEADOS
    uint8_t all_blocked = 1;
    while (aux_node != NULL )
    {
        shell_handler(aux_node);
        if(aux_node->pcb->pid != 1 &&aux_node->pcb->state == BLOCKED){
            uint8_t flag = 0;
            pid_node *aux = aux_node->pcb->child_pid_list;
            while(aux != NULL){
                if(get_process_node(aux->pid) != NULL){
                    flag =1;
                    break;
                }
                aux = aux->next;
            }
            if(flag == 0){
                aux_node->pcb->state = READY;
            }
        }
        if(aux_node->pcb->auxPriority > 0 && aux_node->pcb->state != BLOCKED){
            scheduler->current = aux_node;
            scheduler->current->pcb->auxPriority--;
            return scheduler->current->pcb->stackPointer;
        }
        if(aux_node->pcb->state != BLOCKED)
            all_blocked = 0;
        prev_aux_node = aux_node;   
        aux_node = aux_node->next; 

    }

    //SI TODOS LOS PROCESOS ESTAN BLOQUEADOS CORRO EL DEFUALT
    if(all_blocked)
        return starting_process->pcb->stackPointer;

    //RESETEO SI TODAS LAS AUX PRIORITY ESTAN EN CERO
    aux_node = scheduler->process_list;
    while(aux_node != NULL)
    {
        aux_node->pcb->auxPriority = aux_node->pcb->priority;
        aux_node = aux_node->next;
    }
    aux_node = scheduler->process_list;
    while(aux_node->pcb->state == BLOCKED)
        aux_node = aux_node->next;
    scheduler->current = aux_node;
    scheduler->current->pcb->auxPriority--;
    return scheduler->current->pcb->stackPointer;    
}

void send_EOF(int process_id){
    process_node *node = get_process_node(process_id);
    if(node != NULL && node->pcb->stdin_fd != NULL){
        //SEND EOF
    }
}
int kill_process(int process_id){

    //BUSCO EL NODO A REMOVER
    process_node* process_to_remove = scheduler->process_list;
    process_node* prev_process_to_remove = scheduler->process_list;
    while (process_to_remove != NULL)
    {
        if(process_to_remove->pcb->pid == process_id)
            break;
        else{
            prev_process_to_remove = process_to_remove;
            process_to_remove = process_to_remove->next;
        }
    }
    //RETORNO 1 SI NO LO ENCONTRO
    if(process_to_remove == NULL)
        return 1;

    //LO DESCONECTO DE LA LISTA
    if(process_to_remove != prev_process_to_remove){
        prev_process_to_remove->next = process_to_remove->next;
    }else{
        scheduler->process_list = process_to_remove->next;
    }
    
    //LIBERO SUS RECURSOS
    free_fd(process_to_remove->pcb->stdin_fd,process_id);
    free_fd(process_to_remove->pcb->stdout_fd, process_id);
    free_fd_list(process_to_remove->pcb->fds,process_id);
    free_child_pid_list(process_to_remove->pcb->child_pid_list);
    free(process_to_remove->pcb);
    free(process_to_remove);
    force_timer();
    return 0;
}


void free_fd_list(fdNode* fdNode_to_free, int process_id){
    fdNode* aux_node = fdNode_to_free;
    while (aux_node != NULL) {
        fdNode* next_node = aux_node->next;
        free_fd(aux_node->file_descriptor,process_id);
        free(aux_node);
        aux_node = next_node;    
    }
}   
int get_fd_id(fd * my_fd, int pid) {
    process_node * node = get_process_node(pid);
    if(my_fd == node->pcb->stdin_fd) {
        return STDIN;
    } else if (my_fd == node->pcb->stdout_fd){
        return STDOUT;
    }
    return find_fd_id(node->pcb->fds, my_fd);
}

int find_fd_id(fdNode * list, fd * my_fd) {
    if(list == NULL)
        return -1;
    if(list->file_descriptor == my_fd)
        return list->file_descriptor->id;
    return find_fd_id(list->next, my_fd);
}

void free_child_pid_list(pid_node* child_list_to_free){
    pid_node* aux_node = child_list_to_free;
    while (aux_node != NULL) {
        pid_node* next_node = aux_node->next;
        free(aux_node);
        aux_node = next_node;    
    }
}  

void close_fd(unsigned int fd, int pid) {
    process_node * node = get_process_node(pid);
    if(STDIN == fd) {
        free_fd(node->pcb->stdin_fd, pid);
    } else if (STDOUT == fd){
         free_fd(node->pcb->stdout_fd, pid);
    }

    node->pcb->fds = remove_fd_from_list(node->pcb->fds,fd, pid);
    return;
}

fd * create_fd(int pid) {
    process_node * node = get_process_node(pid);

    if(node == NULL)
        return NULL;
    
    fdNode * new_node = (fdNode *) alloc(sizeof(fdNode));
    if(new_node == NULL)
        return NULL;
    
    fd * new_fd = (fd *) alloc(sizeof(fd));
    if(new_fd == NULL)
        return NULL;

    new_fd->id = node->pcb->next_fd_id;
    node->pcb->next_fd_id++;
    new_fd->pipe = NULL;
    new_fd->readable = 0;
    new_fd->writable = 0;

    new_node->file_descriptor = new_fd;
    new_node->next = NULL;


    if(node->pcb->fds == NULL) {
        node->pcb->fds = new_node;
        node->pcb->last_node = node->pcb->fds;
    }

    node->pcb->last_node->next = new_node;
    node->pcb->last_node = node->pcb->last_node->next;

    return new_fd;
}

//agregar arriba de todo
fdNode * remove_fd_from_list(fdNode* list, unsigned int fd, int pid) {
    if(list == NULL)
        return list;
    if(list->file_descriptor->id == fd) {
        fdNode * aux = list->next;
        free_fd(list->file_descriptor, pid);
        free(list);
        return aux;
    }
    list->next = remove_fd_from_list(list->next, fd, pid);
    return list;
}


int dup_fd(unsigned int dest_fd, unsigned int src_fd, int pid) {
    process_node * node = get_process_node(pid);
    if(node == NULL)
        return -1;
    
    fdNode * aux;
    fd * dest;
    fd * src;
    if(dest_fd == STDIN) {
        dest = node->pcb->stdin_fd;
    } else if(dest_fd == STDOUT) {
        dest = node->pcb->stdout_fd;
    } else {
        aux = get_fd_node(node->pcb->fds, dest_fd);
        if(aux == NULL)
            return -1;
        dest = aux->file_descriptor;
        if(dest == NULL) 
            return -1;
    }

    if(src_fd == STDIN) {
        src = node->pcb->stdin_fd;
    } else if(src_fd == STDOUT) {
        src = node->pcb->stdout_fd;
    } else {
        aux = get_fd_node(node->pcb->fds, src_fd);
        if(aux == NULL)
            return -1;
        src = aux->file_descriptor;
        if(src == NULL)
            return -1;
    }
    
    if(dest->pipe != NULL) {
        pipe_close(dest->pipe, pid);
    }
    if(src->pipe != NULL) {
        pipe_open(dest, get_pipe_id(src->pipe), pid);
    }
    dest->pipe = src->pipe;
    dest->writable = src->writable;
    dest->readable = src->readable;
    return 0;
}


fdNode * get_fd_node(fdNode * list, unsigned int fd_id) {
    if(list == NULL)
        return list;
    if(list->file_descriptor->id == fd_id) {
        return list;
    }
    return get_fd_node(list->next, fd_id);
}

void free_fd(fd* fd_to_free,int process_id){
    if(fd_to_free == NULL)  
        return;
    if(fd_to_free->pipe != NULL){
        pipe_close(fd_to_free->pipe,process_id);
    }
}

process_node* get_process_node(int process_id){
    process_node* node = scheduler->process_list;
    while (node != NULL)
    {
        if(node->pcb->pid == process_id)
            return node;
        else
            node = node->next;
    }
    return NULL;
}

fd * get_stdout(int pid) {
    process_node * node = get_process_node(pid);
    if(node == NULL)
        return NULL;
    
    return node->pcb->stdout_fd;
}

fd * get_stdin(int pid) {
    process_node * node = get_process_node(pid);
    if(node == NULL)
        return NULL;
    
    return node->pcb->stdin_fd;
}


fd * get_fd(int pid, unsigned int searching_fd) {
    return get_fd_node(get_process_node(pid)->pcb->fds, searching_fd)->file_descriptor;
}

int free_process_resource(process_node *process){
    
    free(process->pcb);
    free(process);
    return 0;
}

int get_PID(){
    return scheduler->current->pcb->pid;
}


void normalizeSpaces(char *buf, char *data, int field) {
  static int fields[] = {10, 2, 8, 8, 8, 10, 7};
  int n = fields[field] - my_strlen(data);
  my_strcat(buf, data);
  for (int i = 0; i < n; i++) {
    my_strcat(buf, " ");
  }
  my_strcat(buf, "  ");
}


void get_process_list(char *buf) {
  my_strcat(buf, "Name        ID  Priority  RSP       RBP       Foreground  State\n");
  process_node * current = scheduler->process_list;
  while (current != NULL) {
    normalizeSpaces(buf, current->pcb->args, NAME);

    uint32_t pid = current->pcb->pid;
    char pidStr[6];
    cUintToBase(pid, pidStr, 10);
    normalizeSpaces(buf, pidStr, ID);

    uint32_t priority = current->pcb->priority;
    char priorityStr[3];
    cUintToBase(priority, priorityStr, 10);
    normalizeSpaces(buf, priorityStr, PRIORITY);

    uint32_t rsp = current->pcb->stackPointer;
    char rspStr[10];
    cUintToBase(rsp, rspStr, 16);
    normalizeSpaces(buf, rspStr, RSP);

    uint32_t rbp = current->pcb->basePointer;
    char rbpStr[10];
    cUintToBase(rbp, rbpStr, 16);
    normalizeSpaces(buf, rbpStr, RBP);

    normalizeSpaces(buf, priority == 1 ? "yes" : "no", FOREGROUND);

    uint8_t state = current->pcb->state;
    normalizeSpaces(buf, state == 1 ? "running" : "blocked", STATE);

    my_strcat(buf, "\n");
    current = current->next;
  }
}

int change_priority(int process_id,int priority){
    //BUSCO EL PROCESO
    process_node* node = get_process_node(process_id);

    //SINO LO ENCUENTRO RETORNO 1
    if(node == NULL)
        return 1;

    //CAMBIO LA PRIORIDAD
    node->pcb->priority = priority;

    //REUBICO EL PROCESO EN LA LISTA PARA QUE SE MANTENGA ORDENADA
    bubbleSort(scheduler->process_list);

    return 0;
}
int block_process(int process_id){
    process_node* node = get_process_node(process_id);
    node->pcb->state = BLOCKED;
    return 0;
}
int unblock_process(int process_id){
    process_node* node = get_process_node(process_id);
    node->pcb->state = READY;
    return 0;
}
int cede_cpu(int process_id){
    process_node* node = get_process_node(process_id);
    if(node->pcb->state == RUNNING){
        node->pcb->auxPriority = 0;
    }
    return 0;
}

int wait_pid(int process_id){
    process_node* node = get_process_node(process_id);
    pid_node* aux_node_pid = node->pcb->child_pid_list;
    while (aux_node_pid != NULL){
        process_node* child_node = get_process_node(aux_node_pid->pid);
        if(child_node != NULL){
            node->pcb->state = BLOCKED;  
            force_timer();      
        }
        aux_node_pid = aux_node_pid->next;
    }
    return 0;
}
int create_child_process(uint64_t ip) {  
    // process_node* node = scheduler->current;

    // fd* stdin_cpy = (fd*) alloc(sizeof(fd));
    // stdin_cpy->pipe = node->pcb->stdin_fd->pipe;
    // stdin_cpy->shared_mem = node->pcb->stdin_fd->shared_mem;
    // stdin_cpy->readable = node->pcb->stdin_fd->readable;
    // stdin_cpy->writable = node->pcb->stdin_fd->writable;
    // stdin_cpy->pipe = node->pcb->stdin_fd->pipe;

    // fd* stdout_cpy = (fd*) alloc(sizeof(fd));
    // stdout_cpy->pipe = node->pcb->stdout_fd->pipe;
    // stdout_cpy->shared_mem = node->pcb->stdout_fd->shared_mem;
    // stdout_cpy->readable = node->pcb->stdout_fd->readable;
    // stdout_cpy->writable = node->pcb->stdout_fd->writable;
    // stdout_cpy->pipe = node->pcb->stdout_fd->pipe;

    // fd* 
    // while(){
    //     fd* stdout_cpy = (fd*) alloc(sizeof(fd));
    //     stdout_cpy->pipe = node->pcb->stdout_fd->pipe;
    //     stdout_cpy->shared_mem = node->pcb->stdout_fd->shared_mem;
    //     stdout_cpy->readable = node->pcb->stdout_fd->readable;
    //     stdout_cpy->writable = node->pcb->stdout_fd->writable;
    //     stdout_cpy->pipe = node->pcb->stdout_fd->pipe;
    // }
    // int create_process(uint64_t ip, uint8_t priority, uint64_t argc,char argv[MAX_ARGS][MAX_MAX_ARG_LENGTHGTH], fd *customStdin,fd *customStdout)
    
    // create_process(ip,,node->pcb->args,)
    return 0;
}
  
  
/* Bubble sort the given linked list */
void bubbleSort(process_node *start) 
{ 
    int swapped, i; 
    process_node *ptr1; 
    process_node *lptr = NULL; 
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next != lptr) 
        { 
            if (ptr1->pcb->priority < ptr1->next->pcb->priority) 
            { 
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 
  
/* function to swap data of two nodes a and b*/
void swap(process_node *a, process_node *b) 
{ 
    int temp = a->pcb->priority; 
    a->pcb->priority = b->pcb->priority; 
    b->pcb->priority = temp; 
} 