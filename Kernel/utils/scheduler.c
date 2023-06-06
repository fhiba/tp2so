#include <stdio.h>
#include <scheduler.h>
#include <naiveConsole.h>

#define DEFAULT_PROG_MEM 4096

typedef struct process_node {
    pcb *pcb;
    struct process_node *next
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

#define BLOCKED 0
#define READY 1
#define RUNNING 2
#define EXITED 3

void insert_by_priority(process_node ** head, process_node* newNode);

/* Function to bubble sort the given linked list */
void bubbleSort(process_node *start); 
  
/* Function to swap data of two nodes a and b*/
void swap(process_node *a, process_node *b); 
 
void insert_in_pid_list(int ppid,int pid);


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

static char *strcpy(char *destination, const char *source) {
  if (destination == NULL) return NULL;

  char *ptr = destination;

  while (*source != '\0') {
    *destination = *source;
    destination++;
    source++;
  }

  *destination = '\0';
  return ptr;
}

int create_process(uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout, uint8_t background){
    //CREO EL PROCESO
    pcb *newPCB = (pcb *)alloc(sizeof(pcb));
    newPCB->pid = currentPID++;
    newPCB->priority = priority;
    newPCB->auxPriority = priority;
    newPCB->stdin_fd = customStdin;
    newPCB->stdout_fd = customStdout;
    newPCB->state = 1;
    newPCB->background = background;
    for (int i = 0; i < argc; i++) {
        strcpy(newPCB->args[i], argv[i]);
    }
    uint64_t processMemory = (uint64_t)alloc(DEFAULT_PROG_MEM);
    uint64_t sp = initProcess(processMemory + DEFAULT_PROG_MEM, ip, argc, newPCB->args);
    newPCB->stackPointer = sp;
    newPCB->basePointer = processMemory + DEFAULT_PROG_MEM - 1;  // no se si aca falta un -1
    newPCB->processMemory = processMemory;
    pid_node*new_child_pid_list = NULL;
    process_node *newNode = (process_node *)alloc(sizeof(process_node));
    newNode->pcb = newPCB;
    insert_by_priority(&(scheduler->process_list),newNode);
    force_timer();
    return newPCB->pid;
}  

int create_child(int ppid,uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout, uint8_t background){

    int child_pid = create_process(ip,priority,argc,argv,customStdin,customStdout,background);
    
    process_node* parent_node = get_process_node(child_pid);

    insert_in_pid_list(ppid,child_pid);
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
    if (*head == NULL || (*head)->pcb->priority <= newNode->pcb->priority)
    {
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
    free_fd(process_to_remove->pcb->stdout_fd,process_id);
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

void free_child_pid_list(pid_node* child_list_to_free){
    pid_node* aux_node = child_list_to_free;
    while (aux_node != NULL) {
        pid_node* next_node = aux_node->next;
        free(aux_node);
        aux_node = next_node;    
    }
}  

void free_fd(fd* fd_to_free,int process_id){
    if(fd_to_free != NULL){
        if(fd_to_free->pipe != NULL){
            //close_pipe(fd_to_free->pipe,process_id);
        }
        free(fd_to_free);
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


int free_process_resource(process_node *process){
    
    free(process->pcb);
    free(process);
}

int get_PID(){
    return scheduler->current->pcb->pid;
}

void get_process_list(){
    ncPrint("NAME      PID       PRIORITY      STACK     BASE POINTER      FOREGROUND");
    process_node * iter = scheduler->process_list;
    while(iter != NULL){
        ncPrint(iter->pcb->args);//name
        ncPrint("      ");
        ncPrint(iter->pcb->pid);//pid
        ncPrint("      ");
        ncPrint(iter->pcb->priority);//priority
        ncPrint("      ");
        ncPrint(iter->pcb->stackPointer);//stack
        ncPrint("      ");
        ncPrint(iter->pcb->basePointer);//stack
        ncPrint("      ");
        ncPrint(iter->pcb->fds);//stack
        ncPrint("      ");
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
int create_child_process(uint64_t ip){  
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
    // int create_process(uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout)
    
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