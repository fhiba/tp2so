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



static void default_process() {
  while (1) {
    _hlt();
  }
}

void initScheduler(){
    scheduler = (Scheduler *)alloc(sizeof(Scheduler));
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
        //SI ALGUN PROCESO ESTA EXITED CAMBIO SU PRIORIDAD A CERO Y LO MANDO AL FONDO DE LA LISTA PARA NUNCA MAS SEA CORRIDO
        if(aux_node->pcb->state == EXITED){
            aux_node->pcb->priority = 0;
            aux_node->pcb->auxPriority = 0;
            process_node *go_to_end_node = aux_node;
            while (go_to_end_node->next != NULL)
                go_to_end_node = go_to_end_node->next;
            prev_aux_node->next = aux_node->next;
            go_to_end_node->next = aux_node;
            aux_node->next = NULL;
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
        if(aux_node->pcb->state != EXITED){
            aux_node->pcb->auxPriority = aux_node->pcb->priority;
            aux_node = aux_node->next;
        }
    }

    scheduler->current = scheduler->process_list;
    scheduler->current->pcb->auxPriority--;
    return scheduler->current->pcb->stackPointer;    
}
int kill_process(int process_id){
    return 0;
}
int get_PID(){
    return 0;
}
pcb* get_process_list(){
 
}
int change_priority(int process_id,int priority){
    return 0;
}
int block_process(int process_id){
    return 0;
}
int unblock_process(int process_id){
    return 0;
}
int cede_cpu(int process_id){
    return 0;
}
int wait_pid(int process_id){
    return 0;
}
int fork(){
    return 0;
}