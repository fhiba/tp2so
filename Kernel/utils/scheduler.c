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

int create_process(uint64_t ip, uint8_t priority, uint64_t argc,
                  char argv[ARG_QTY][ARG_LEN], fd *customStdin,
                  fd *customStdout){

    //CREO EL PROCESO
    if (scheduler->process_list == NULL) {
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
        uint64_t sp = initProcess(processMemory + DEFAULT_PROG_MEM, ip, argc,
                                newPCB->args);
        newPCB->stackPointer = sp;
        newPCB->basePointer = processMemory + DEFAULT_PROG_MEM - 1;  // no se si aca falta un -1
        newPCB->processMemory = processMemory;
        process_node *newNode = (process_node *)alloc(sizeof(process_node));
        newNode->pcb = newPCB;
        scheduler->process_list = newNode;
        return 0;
    }
    // pcb *new_process = (pcb *)alloc(sizeof(pcb));
    // new_process->pid = currentPID++;
    // new_process->priority = priority;
    // new_process->auxPriority = priority;
    // new_process->stdin = customStdin;
    // new_process->stdout = customStdout;
    // new_process->state = 1;
    // for (int i = 0; i < argc; i++) {
    //   strcpy(new_process->args[i], argv[i]);
    // }

    // //LO INSERTO EN LA LISTA DE PROCESOS
    // process_node *new_process_node = (process_node *)alloc(sizeof(process_node));
    // new_process_node->pcb = new_process;
    // process_node* aux_list = scheduler->process_list;
    // while(aux_list->next != NULL)
    //     aux_list = aux_list->next;
    // aux_list->next = new_process_node;

    return 0;
}  
int switch_context(int rsp){
    if (firstProcess) {
        firstProcess = 0;
        scheduler->current = scheduler->process_list;
        return scheduler->process_list->pcb->stackPointer;
    }
    
    scheduler->current->pcb->stackPointer = rsp;
    if(scheduler->current->next == NULL){
        scheduler->current = scheduler->process_list;
    }else{
        scheduler->current = scheduler->current->next;
    }
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