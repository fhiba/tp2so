#include <stdio.h>

#include <scheduler.h>


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

int create_process(uint64_t ip, uint8_t priority, uint64_t argc,char argv[ARG_QTY][ARG_LEN], fd *customStdin,fd *customStdout){
    //CREO EL PROCESO
    pcb *newPCB = (pcb *)alloc(sizeof(pcb));
    newPCB->pid = currentPID++;
    newPCB->priority = priority;
    newPCB->auxPriority = priority;
    newPCB->stdin = customStdin;
    newPCB->stdout = customStdout;
    newPCB->state = 1;
    for (int i = 0; i < argc; i++) {
        strcpy(newPCB->args[i], argv[i]);
    }
    uint64_t processMemory = (uint64_t)alloc(DEFAULT_PROG_MEM);
    uint64_t sp = initProcess(processMemory + DEFAULT_PROG_MEM, ip, argc, newPCB->args);
    newPCB->stackPointer = sp;
    newPCB->basePointer = processMemory + DEFAULT_PROG_MEM - 1;  // no se si aca falta un -1
    newPCB->processMemory = processMemory;
    process_node *newNode = (process_node *)alloc(sizeof(process_node));
    newNode->pcb = newPCB;
    insert_by_priority(&(scheduler->process_list),newNode);
    return 0;
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
    scheduler->current = scheduler->process_list;
    scheduler->current->pcb->auxPriority--;
    return scheduler->current->pcb->stackPointer;    
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
        firstProcess = 1;
    }
    
    //LIBERO SUS RECURSOS
    free_fd(process_to_remove->pcb->stdin,process_id);
    free_fd(process_to_remove->pcb->stdout,process_id);
    free_fd_list(process_to_remove->pcb->fds,process_id);
    free(process_to_remove->pcb);
    free(process_to_remove);
    return 0;
}

void free_fd_list(fdNode* fdNode_to_free, int process_id){
    fdNode* aux_node = fdNode_to_free;
    while (aux_node != NULL) {
        fdNode* next_node = aux_node->next;
        free_fd(fdNode_to_free->file_descriptor,process_id);
        free(fdNode_to_free);
        aux_node = next_node;    
    }
}   

void free_fd(fd* fd_to_free,int process_id){
    if(fd_to_free->pipe != NULL){
        //close_pipe(fd_to_free->pipe,process_id);
    }else if(fd_to_free->shared_mem != NULL){
        //close_shm(fd_to_free->pipe,process_id);
    }
    free(fd_to_free);
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
    printf("NAME      PID       PRIORITY      STACK     BASE POINTER      FOREGROUND");
    process_node * iter = scheduler->process_list;
    while(iter != NULL){
        printf(iter->pcb->args);//name
        printf("      ");
        printf(iter->pcb->pid);//pid
        printf("      ");
        printf(iter->pcb->priority);//priority
        printf("      ");
        printf(iter->pcb->stackPointer);//stack
        printf("      ");
        printf(iter->pcb->basePointer);//stack
        printf("      ");
        printf(iter->pcb->fds);//stack
        printf("      ");
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
    return 0;
}
int create_child_process(uint64_t ip){  
    // process_node* node = scheduler->current;

    // fd* stdin_cpy = (fd*) alloc(sizeof(fd));
    // stdin_cpy->pipe = node->pcb->stdin->pipe;
    // stdin_cpy->shared_mem = node->pcb->stdin->shared_mem;
    // stdin_cpy->readable = node->pcb->stdin->readable;
    // stdin_cpy->writable = node->pcb->stdin->writable;
    // stdin_cpy->pipe = node->pcb->stdin->pipe;

    // fd* stdout_cpy = (fd*) alloc(sizeof(fd));
    // stdout_cpy->pipe = node->pcb->stdout->pipe;
    // stdout_cpy->shared_mem = node->pcb->stdout->shared_mem;
    // stdout_cpy->readable = node->pcb->stdout->readable;
    // stdout_cpy->writable = node->pcb->stdout->writable;
    // stdout_cpy->pipe = node->pcb->stdout->pipe;

    // fd* 
    // while(){
    //     fd* stdout_cpy = (fd*) alloc(sizeof(fd));
    //     stdout_cpy->pipe = node->pcb->stdout->pipe;
    //     stdout_cpy->shared_mem = node->pcb->stdout->shared_mem;
    //     stdout_cpy->readable = node->pcb->stdout->readable;
    //     stdout_cpy->writable = node->pcb->stdout->writable;
    //     stdout_cpy->pipe = node->pcb->stdout->pipe;
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