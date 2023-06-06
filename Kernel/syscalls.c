#include <syscalls.h>






#define RETVALUE -1



static char snapshot = 0;
static const char* registerNames[17] = {
    "RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"
};

extern uint64_t registers[17];
extern char registersSaved;



void write_tty(int color_choice, char*buffer, size_t count) {
    /*int pid = get_PID();
    pcb * process = get_pcb(pid);
    if(process->background)         
        return;
        
    */

    int color = 0;
    for(int i =0;i<count;i++){
        if(buffer[i] == '\n'){
            ncNewline();
        } else if (buffer[i] == 0x7F) {
            deleteChar();
        } else {
            switch(color_choice){
            case 1:
                color = BLANCO;
                break;
            case 2:
                color = ROJO;
                break;
            case 3:
                color = VERDE;
                break;
            case 4:
                color = CELESTE;
                break;
            }
            drawChar(buffer[i],color,NEGRO);
        }
    }

}

void write(unsigned int dest_fd, int color_choice, char*buffer, size_t count){
    int pid = get_PID();
    fd * my_fd = NULL;
    if(dest_fd == STDOUT) {
        my_fd = get_stdout(pid);
    } else {
        my_fd = get_fd(pid, dest_fd);
    }
    if(my_fd == NULL || my_fd->writable == 0)
        return;
    if(my_fd->pipe != NULL) {
        write_pipe(my_fd, buffer, count);
    } else {
        write_tty(color_choice, buffer, count);
    }
}

int read(unsigned int dest_fd, char * buffer, size_t count){
    int pid = get_PID();
    fd *  my_fd = NULL;
    
    if(dest_fd == STDIN) {
        my_fd = get_stdin(pid);
    } else {
        my_fd = get_fd(pid, dest_fd);
    }

    if(my_fd == NULL || my_fd->readable == 0)
        return -1;

    if(my_fd->pipe != NULL)
        return read_pipe(my_fd, buffer, count);
    else {
        int k = 0;
        unsigned char key = 0;
        while(k < count ){
            _hlt();
            key = readKey();
            buffer[k++] = key;
        }
        return k;
    }
    return -1;
}

void clear(){
    ncClear();
}

void resize(int num){
    nResize(num);
}

char mydate(char value){
    return rtcGet(value);
}



void inforegs(){
    if(!registersSaved){
        write(STDOUT, 2,"Primero debe tomar un snapshot con la tecla ctrl",49);
        ncNewline();
    }else{
        for(int i = 0;i<17;i++){
            write(STDOUT,4,registerNames[i],3);
            write(STDOUT, 1,": ",2);
            ncPrintBase(registers[i],16);
            ncNewline();
        }
    }
}

uint64_t xtou64(const char *str)
{
    uint64_t res = 0;
    char c;

    while ((c = *str++)) {
        char v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8);
        res = (res << 4) | (uint64_t) v;
    }

    return res;
} 

void printMem(uint64_t pointer, unsigned char * buf)
{
	uint8_t *start = (uint8_t *)pointer;
	for (int i = 0; i < 32; i++)
	{
		buf[i] = start[i];
	}
}

void printBase(uint64_t value, uint32_t base) {
    ncPrintBase(value, base);
}

void memPrint(uint64_t pointer, unsigned char * buf)
{
	uint8_t *start = (uint8_t *)pointer;
	for (int i = 0; i < 20; i++)
	{
		buf[i] = start[i];
	}
}

void new_pipe(unsigned int fds[2]) {
    int pid = get_PID();
    fd * new_readable_fd = create_fd(pid);

    if(new_readable_fd == NULL)
        return;

    new_readable_fd->readable = 1;
    fd * new_writable_fd = create_fd(pid);

    if(new_writable_fd == NULL) {
        close(new_readable_fd);
        return;
    }
    new_writable_fd->writable = 1;

    int pipe_id = pipe_create(0);
    if(pipe_id < 0) {
        close(new_writable_fd);
        close(new_readable_fd);
        return;
    }

    uint8_t status = pipe_open(new_readable_fd, pipe_id, pid);

    if(status > 0) {
        destroy_pipe(get_pipe(pipe_id));
        close(new_writable_fd);
        close(new_readable_fd);
        return;
    }
        
    status = pipe_open(new_writable_fd, pipe_id, pid);
    if(status > 0) {
        pipe_close(new_readable_fd->pipe, pid);
        close(new_writable_fd);
        close(new_readable_fd);
        return;
    }

    fds[0] = new_readable_fd->id; //read en 0
    fds[1] = new_writable_fd->id; //write en 1
    return;
}


void close(unsigned int fd) {
    int pid = get_PID();
    close_fd(fd, pid);
}

uint8_t create_named_pipe(char * name) {
    int pid = get_PID();
    return named_pipe_create(name) > 0 ? 1: 0;
}

uint8_t named_pipe_destroy(char * name) {
    return destroy_named_pipe(name);
}

unsigned int named_pipe_open(char * name, uint8_t operation) {
    int pid = get_PID();

    fd * new_fd = create_fd(pid);

    if(new_fd == NULL)
        return;

    if(operation == READ) {
        new_fd->readable = 1;        
    } else {
        new_fd->writable = 1;
    }
    uint8_t status = open_named_pipe(new_fd, name, pid);

    if(status > 0) {
        close(new_fd);
        return;
    }
    return new_fd->id;
}


void dup(unsigned int dest_fd, unsigned int src_fd) {
    int pid = get_PID();
    dup_fd(dest_fd, src_fd, pid);
}