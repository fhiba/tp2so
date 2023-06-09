GLOBAL cpuVendor
GLOBAL getKey
EXTERN saveRegisters
EXTERN registersSaved
GLOBAL rtcGet
GLOBAL getDir
GLOBAL inb
GLOBAL outb
EXTERN registers
section .text


%macro saveRegisters 0
    cli
    mov [registers+1*8], rax
    mov [registers+2*8], rbx
    mov [registers+3*8], rcx
    mov [registers+4*8], rdx
    mov [registers+5*8], rsi
    mov [registers+6*8], rdi
    mov [registers+7*8], rbp

    ; rsp
    mov rax, rsp
    add rax, 120
    mov [registers+8*8], rax

    mov [registers+9*8], r8
    mov [registers+10*8], r9
    mov [registers+11*8], r10
    mov [registers+12*8], r11
    mov [registers+13*8], r12
    mov [registers+14*8], r13
    mov [registers+15*8], r14
    mov [registers+16*8], r15

    ; rip
    mov rax, [rsp]
    mov [registers], rax

    mov byte [registersSaved], 1

    sti

%endmacro
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

getKey:
        push rbp
        mov rbp, rsp

        mov rax,0           ;kbFlag

        in al,0x64          ;guardo en al el status del teclado 
        and al,0x01         ;hago la mascara del status para obetener el bit de input disponible
        cmp al,1            ;verifico que esté prendido el bit de input disponible
        jne .end            ;si no lo esta salto al final(la funcion devuelve scancode 0)
        in al,0x60          ;en caso de haber input lo cargo en el registro a
		cmp al, 0b10011101
    	jne .end
    	saveRegisters 


.end    mov rsp, rbp
        pop rbp
        ret

rtcGet:
	push rbp
	mov rbp, rsp

	mov al, dil
	out 0x70, al
	in al, 0x71

	mov rsp, rbp
	pop rbp
	ret



	

;Beep

inb:
    push rbp
    mov rbp, rsp
    mov rdx,rdi    ;puerto
    in al,dx       ;valor del puerto
    leave
    ret

outb:
    push rbp
    mov rbp, rsp
    mov rax, rsi     ;valor    
    mov rdx, rdi     ;puerto
    out dx, al
    leave
    ret
