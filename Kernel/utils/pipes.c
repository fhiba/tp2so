#include <pipes.h>

#define BUFFER_SIZE 512

typedef struct process_node{
    int pid;
    int operation;
    process_node * next;
}process_node;

typedef process_node * process_list;

typedef struct pipe{
    int id;
    char buffer[BUFFER_SIZE];
    unsigned int read_index;
    unsigned int write_index;
    process_list working_processes;
    process_node * last_process_node;
    //semaphore sem_write;
}pipe;

pipe * pipe_array[MAX_PIPES];
int asigned_pipes = 0;


uint8_t add_process_to_pipe(pipe* my_pipe, int pid, int operation);
process_list remove_node(process_list list, int pid, uint8_t * status);
uint8_t remove_process_from_pipe(pipe * my_pipe, int pid);

int pipe_create() {
    if(asigned_pipes == MAX_PIPES)
        return -1;
    int idx = 0;
    
    //busqueda de posición en el array para guardar el pipe
    while(pipe_array[idx] != NULL)
        idx++;

    //alloc mem
    pipe_array[idx] = (pipe*) alloc(sizeof(pipe));
    
    //not enough mem => should exit
    if(pipe_array[idx] == NULL)
        return -1;
    
    //asigning fds array
    //setting up struct
    pipe_array[idx]->read_index = 0;
    pipe_array[idx]->write_index = 0;
    pipe_array[idx]->working_processes = NULL;
    pipe_array[idx]->last_process_node = NULL;
    pipe_array[idx]->id = idx;

    //setting up return values
    return idx;
}

uint8_t pipe_open(fd * asigned_fd, int pipe_id, int pid) {
    if(pipe_id < 0 || pipe_id > MAX_PIPES)
        return 1;
    if(pipe_array[pipe_id] == NULL)
        return 1;
    if(asigned_fd == NULL)
        return 1;    
    asigned_fd->pipe = pipe_array[pipe_id];
     
    return add_process_to_pipe(pipe_array[pipe_id], pid, asigned_fd->readable? READ:WRITE);
}

uint8_t pipe_close(pipe * my_pipe, int pid) {
    if(my_pipe == NULL)
        return 1;
    
    if(remove_process_from_pipe(my_pipe, pid) != 0) 
        return 1;
    if(destroyable_pipe(my_pipe)) {
        return destroy_pipe(my_pipe);
    }

    return 0;
}

uint8_t remove_process_from_pipe(pipe * my_pipe, int pid) {
    
    if(my_pipe == NULL)
        return 1;

    process_node * list = my_pipe->working_processes;
    uint8_t status = 1;
    list = remove_node(list, pid, &status);
    return status;
}

process_list remove_node(process_list list, int pid, uint8_t * status) {
    if(list == NULL)
        return NULL;
    if(list->pid == pid) {
        *status = 0;
        process_node * next = list->next;
        free(list);
        return next;
    }
    list = remove_node(list->next, pid, status);

    return list;
}

uint8_t add_process_to_pipe(pipe * my_pipe, int pid, int operation) {
    process_node * new_node = (process_node *) alloc(sizeof(process_node));
    if(new_node == NULL)
        return 1;
    new_node->pid = pid;
    new_node->operation = operation;

    if(my_pipe->working_processes == NULL) {
        my_pipe->working_processes = new_node;
        my_pipe->last_process_node = my_pipe->working_processes;
    }
    my_pipe->last_process_node->next = new_node;
    my_pipe->last_process_node = my_pipe->last_process_node->next;
    return 0;
}

uint8_t destroyable_pipe(pipe * my_pipe) {
    if(my_pipe->working_processes == NULL)
        return 1;
    return 0;    
}

uint8_t destroy_pipe(pipe * my_pipe) {
    if(!destroyable_pipe(my_pipe))
        return 1;
    int idx = my_pipe->id;
    pipe_array[idx] = NULL;
    free(my_pipe);
    return 0;   
}

int read_pipe(fd * user_fd, char * buffer, int max_bytes) {
    if(user_fd == NULL || user_fd->readable == 0)
        return -1;
    pipe * pipe_to_read = user_fd->pipe;
    
    if(pipe_to_read == NULL)
        return -1;
    
    int read_bytes = 0;

    while(read_bytes < max_bytes) {
        if(pipe_to_read->read_index != pipe_to_read->write_index) {
            buffer[read_bytes++] = pipe_to_read->buffer[pipe_to_read->read_index++];
        } else {
            //sem_post(pipe_to_read->sem_write);
            return read_bytes;
        }
    }
    //sem_post(pipe_to_read->sem_write);
    return read_bytes;
}

int write_pipe(fd * user_fd, char * buffer, int max_bytes) {
    if(user_fd == NULL || user_fd->writeable == 0)
        return -1;
    pipe * pipe_to_write = user_fd->pipe;

    if(pipe_to_write == NULL)
        return -1;
    
    int write_bytes = 0;

    while(write_bytes < max_bytes) {
        if(pipe_to_write->read_index != pipe_to_write->write_index) {
            pipe_to_write->buffer[pipe_to_write->write_index++] = buffer[write_bytes++];
        } else {
            //sem_await(pipe_to_read->read_sem);
        }
    }
    return write_bytes;
}
