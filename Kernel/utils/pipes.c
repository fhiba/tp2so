// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipes.h>


#define MAX_PIPES 64





typedef struct named_pipe_node {
    char * name;
    pipe named_pipe;
    struct named_pipe_node * next;
}named_pipe_node;

named_pipe_node * named_pipe_list = NULL;

pipe pipe_array[MAX_PIPES];
int asigned_pipes = 0;

int node_exists(char * name);
named_pipe_node * get_node(named_pipe_node * list, char *name);
uint8_t destroyable_pipe(pipe my_pipe);
named_pipe_node * insert_into_list(named_pipe_node* list, named_pipe_node* insert);
named_pipe_node * remove_node_from_list(named_pipe_node * list, char * name, uint8_t * status);
uint8_t add_process_to_pipe(pipe my_pipe, int pid, int operation);
users_list remove_node(users_list list, int pid, uint8_t * status, uint8_t * signal_close);
uint8_t remove_process_from_pipe(pipe my_pipe, int pid);


int pipe_create(int is_named) {
    if(asigned_pipes == MAX_PIPES)
        return -1;
    int idx = 0;
    
    //busqueda de posición en el array para guardar el pipe
    while(pipe_array[idx] != NULL)
        idx++;

    //alloc mem
    pipe_array[idx] = (pipe) alloc(sizeof(pipe_struct));
    
    //not enough mem => should exit
    if(pipe_array[idx] == NULL)
        return -1;
    
    //asigning fds array
    //setting up struct
    pipe_array[idx]->buffer[0] = 0;
    pipe_array[idx]->read_index = 0;
    pipe_array[idx]->write_index = 0;
    pipe_array[idx]->readable_bytes = 0;
    pipe_array[idx]->working_processes = NULL;
    pipe_array[idx]->last_process_node = NULL;
    pipe_array[idx]->id = idx;
    pipe_array[idx]->is_named = is_named;
    pipe_array[idx]->is_closed = 0;
    pipe_array[idx]->waiting_pid = -1;
    asigned_pipes++;
    
    
    //setting up sems
    

    return idx;
}


int get_pipe_id(pipe my_pipe) {
    int idx = 0;
    while(idx < MAX_PIPES) {
        if(pipe_array[idx] == my_pipe) {
            return idx;
        }
        idx++;
    }
    return -1;
}

pipe get_pipe(int pipe_id) {
    return pipe_array[pipe_id];
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


uint8_t pipe_close(pipe my_pipe, int pid) {
    if(my_pipe == NULL)
        return 1;
    
    if(remove_process_from_pipe(my_pipe, pid) != 0) 
        return 1;
    if(my_pipe->is_named == 0 && destroyable_pipe(my_pipe))
        return destroy_pipe(my_pipe);

    return 0;
}

uint8_t remove_process_from_pipe(pipe my_pipe, int pid) {
    
    if(my_pipe == NULL)
        return 1;
    uint8_t signal_close = 0;
    users_node * list = my_pipe->working_processes;
    uint8_t status = 1;
    list = remove_node(list, pid, &status, &signal_close);
    my_pipe->is_closed = pid != 1 ? signal_close:my_pipe->is_closed;
    return status;
}

users_list remove_node(users_list list, int pid, uint8_t * status, uint8_t * signal_close) {
    if(list == NULL)
        return NULL;
    int list_pid = list->pid;
    if(list->pid == pid) {
        *status = 0;
        *signal_close = list->operation == WRITE ? 1:0;
        users_node * next = list->next;
        free(list);
        return next;
    }
    list = remove_node(list->next, pid, status, signal_close);

    return list;
}

uint8_t add_process_to_pipe(pipe my_pipe, int pid, int operation) {
    users_node * new_node = (users_node *) alloc(sizeof(users_node));
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

uint8_t destroyable_pipe(pipe my_pipe) {
    if(my_pipe->working_processes == NULL)
        return 1;
    return 0;    
}

uint8_t destroy_pipe(pipe my_pipe) {
    int idx = my_pipe->id;
    pipe_array[idx] = NULL;
    if(my_pipe->working_processes != NULL) {
        users_node * aux = my_pipe->working_processes;
        while(aux != NULL) {
            my_pipe->working_processes = aux->next;
            free(aux);
            aux = my_pipe->working_processes;
        }
    }
    free(my_pipe);
    asigned_pipes--;
    return 0;   
}
int read_pipe(fd * user_fd, char * buffer, int max_bytes) {

    if(user_fd == NULL || user_fd->readable == 0)
        return -1;
    pipe pipe_to_read = user_fd->pipe;
    
    if(pipe_to_read == NULL)
        return -1;
    //my_sem_wait(pipe_to_read->sem_pipe_access);
    int read_bytes = 0;

    while(read_bytes < max_bytes) {
        if(pipe_to_read->readable_bytes > 0) {
            buffer[read_bytes++] = pipe_to_read->buffer[pipe_to_read->read_index++];
            pipe_to_read->read_index = pipe_to_read->read_index == BUFFER_SIZE ? 0:pipe_to_read->read_index;
            pipe_to_read->readable_bytes--;
        } else {
            if(pipe_to_read->is_closed && read_bytes == 0)
                return -1;
            if(pipe_to_read->waiting_pid != -1) {
                unblock_pid(pipe_to_read->waiting_pid);
                pipe_to_read->waiting_pid = -1;
            }

            return read_bytes;
            // if(read_bytes > 0 && get_value(pipe_to_read->sem_write) < 1) {
            //     my_sem_post(pipe_to_read->sem_write);
            // }
            // my_sem_post(pipe_to_read->sem_pipe_access);
        }
    }
    if(pipe_to_read->waiting_pid != -1) {
        unblock_pid(pipe_to_read->waiting_pid);
        pipe_to_read->waiting_pid = -1;
    }
    // my_sem_post(pipe_to_read->sem_write);
    // my_sem_post(pipe_to_read->sem_pipe_access);
    return read_bytes;
}

int write_pipe(fd * user_fd, char * buffer, int max_bytes) {
    if(user_fd == NULL || user_fd->writable == 0)
        return -1;
    pipe pipe_to_write = user_fd->pipe;

    if(pipe_to_write == NULL)
        return -1;
    //my_sem_wait(pipe_to_write->sem_pipe_access);
    int write_bytes = 0;

    while(write_bytes < max_bytes) {
        if((pipe_to_write->write_index == 0 && pipe_to_write->buffer[pipe_to_write->write_index] == 0) 
            || pipe_to_write->readable_bytes < BUFFER_SIZE) {
            pipe_to_write->buffer[pipe_to_write->write_index++] = buffer[write_bytes++];
            pipe_to_write->write_index = pipe_to_write->write_index == BUFFER_SIZE ? 0:pipe_to_write->write_index;
            pipe_to_write->readable_bytes++;
        } else {
                if(pipe_to_write->waiting_pid != -1) {
                    unblock_pid(pipe_to_write->waiting_pid);
                    pipe_to_write->waiting_pid = -1;
                } 
                pipe_to_write->waiting_pid = get_PID();
                block_pid(get_PID());
                force_timer();
            // my_sem_post(pipe_to_write->sem_pipe_access);
            // my_sem_wait(pipe_to_write->sem_write);
            // my_sem_wait(pipe_to_write->sem_pipe_access);
        }
    }
    if(pipe_to_write->waiting_pid != -1) {
        unblock_pid(pipe_to_write->waiting_pid);
        pipe_to_write->waiting_pid = -1;
    } 
    //my_sem_post(pipe_to_write->sem_pipe_access);
    return write_bytes;
}

//!------------------------------------------------------------------------------------------------- named pipes funcs

uint8_t open_named_pipe(fd * asigned_fd, char * name , int pid) {
    named_pipe_node * named_pipe = get_node(named_pipe_list, name);

    if(named_pipe == NULL)
        return 1;
    
    return pipe_open(asigned_fd, named_pipe->named_pipe->id, pid);
}

named_pipe_node * get_node(named_pipe_node * list, char *name) {
    if(list == NULL)
        return list;
    
    if(my_strcmp(list->name, name) == 0)
        return list;

    return get_node(list->next, name);
}


uint8_t destroy_named_pipe(char * name) {
    uint8_t status = 0;
    named_pipe_list = remove_node_from_list(named_pipe_list, name, &status);
    return status;
}

named_pipe_node * remove_node_from_list(named_pipe_node * list, char * name, uint8_t * status){ 
    if(list == NULL)
        return NULL;
    int cmp = my_strcmp(list->name, name);
    if(cmp > 0)
        return list;
    else if(cmp < 0) {
        list->next = remove_node_from_list(list->next, name, status);
    }

    named_pipe_node * aux = list->next;
    free(list->name);
    destroy_pipe(list->named_pipe);
    free(list);
    *status = 1;
    return aux;
}

int named_pipe_create(char * name) {
    if(name == NULL)
        return -1;
    
    if(node_exists(name) == 1)
        return -1;


    named_pipe_node * new_pipe = (named_pipe_node *) alloc(sizeof(named_pipe_node));
    
    if(new_pipe == NULL)
        return -1;

    unsigned int len = my_strlen(name);

    new_pipe->name = (char *) alloc(len+1);

    if(new_pipe->name == NULL) {
        free(new_pipe);
        return -1;
    }

    my_strcpy(new_pipe->name, name);

    int idx = pipe_create(1);

    if(idx < 0) {
        free(new_pipe->name);
        free(new_pipe);
    }
    
    new_pipe->next = NULL;

    named_pipe_list = insert_into_list(named_pipe_list, new_pipe);
    return idx;
}

int node_exists(char * name) {
    named_pipe_node * aux = named_pipe_list;
    while(aux != NULL && (my_strcmp(aux->name, name) < 0)) {
        aux = aux->next;
    }
    if(my_strcmp(aux->name, name) == 0)
        return 1;
    return 0;
}

named_pipe_node * insert_into_list(named_pipe_node* list, named_pipe_node* insert) {
    if(list == NULL)
        return insert;
    
    if(my_strcmp(list->name, insert->name) > 0) { 
        insert->next = list;
        return insert;
    }

    list->next = insert_into_list(list->next, insert);

    return list;
}