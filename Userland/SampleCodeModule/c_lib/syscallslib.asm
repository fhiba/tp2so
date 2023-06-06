section .text
GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_printMem
GLOBAL sys_clear
GLOBAL sys_date
GLOBAL sys_sleep
GLOBAl sys_setScreen
GLOBAL sys_infoRegs
GLOBAL sys_getLast
GLOBAL sys_snapshotRegs
GLOBAL sys_resize
GLOBAL sys_divzero
GLOBAL sys_opcode
GLOBAL sys_recto
GLOBAL sys_tron
GLOBAL sys_beep
GLOBAL sys_printBase
GLOBAL sys_isBlackPixel
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_process
GLOBAL sys_kill
GLOBAL sys_get_pid
GLOBAL sys_block
GLOBAL sys_nice
GLOBAL sys_ps
GLOBAL sys_sem_create
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_create_pipe
GLOBAL sys_close_pipe
GLOBAL sys_dup_fd

sys_write:
    mov rax, 2
    int 80h
    ret

sys_read:
    mov rax, 1
    int 80h
    ret

sys_beep:
    mov rax, 17
    int 80h
    ret

sys_printMem:
    mov rax, 40
    int 80h
    ret

sys_clear:
    mov rax, 3
    int 80h
    ret

sys_date:
    mov rax, 6
    int 80h
    ret

sys_sleep:
    mov rax, 7
    int 80h
    ret

sys_process:
    mov rax, 8
    int 80h
    ret

sys_infoRegs:
    mov rax, 5
    int 80h
    ret

sys_snapshotRegs:
    mov rax, 12
    int 80h
    ret

sys_resize:
    mov rax,9
    int 80h
    ret

sys_tron:
    mov rax,13
    int 80h
    ret

sys_divzero:
    mov rax, 14
    int 80h
    ret

sys_opcode:
    mov rax, 15
    int 80h
    ret

sys_recto:
    mov rax,16
    int 80h
    ret

sys_malloc:
    mov rax, 18
    int 80h
    ret

sys_free:
    mov rax,19
    int 80h
    ret


sys_printBase:
    mov rax, 20
    int 80h
    ret

sys_kill:
    mov rax, 21
    int 80h
    ret

sys_get_pid:
    mov rax, 22
    int 80h
    ret

sys_isBlackPixel:
    mov rax, 23
    int 80h
    ret

sys_block:
    mov rax, 24
    int 80h
    ret

sys_nice:
    mov rax, 25
    int 80h
    ret

sys_ps:
    mov rax, 26
    int 80h
    ret

sys_sem_wait:
    mov rax, 27
    int 80h
    ret

sys_sem_post:
    mov rax,28
    int 80h
    ret

sys_sem_create:
    mov rax, 29
    int 80h
    ret

sys_sem_open:
    mov rax, 30
    int 80h
    ret

sys_create_pipe:
    mov rax, 31
    int 80h
    ret

sys_close_pipe:
    mov rax, 32
    int 80h
    ret

sys_dup_fd:
    mov rax, 33
    int 80h
    ret


opcodetest:
    push rbp
    mov rbp, rsp
    mov cr6, rax
    mov rsp, rbp
    pop rbp
    ret

