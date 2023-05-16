
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _syscallHandler
GLOBAL divzero
GLOBAL opcode
; GLOBAL printAllRegs

; EXTERN getStackBase
EXTERN set_syscall
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN sys_dispatcher
EXTERN takeSnapshot
EXTERN inforegs
GLOBAL registersSaved
GLOBAL registers
GLOBAL registersException
; EXTERN saveBackup
; EXTERN ncPrintReg
; EXTERN rebootTerm

; EXTERN printRegPtr	;Este puntero a funcion debe ser desreferenciado para poder ser llamado

SECTION .text

%macro pushStateWithoutRax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pushState 0
	push rax
	pushStateWithoutRax
%endmacro

%macro popStateWithoutRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro popState 0
	popStateWithoutRax
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState
	cli

	; call saveBackup

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al
	popState
	sti
	iretq
%endmacro

%macro exceptionHandler 1
    pushState
	saveRegistersException
    mov rdi, %1 ; pasaje de parametro
    call exceptionDispatcher

    popState

    ; Piso la direccion de retorno para que vuelva al shell.
    push rax
    mov rax, 0x00400000
    mov [rsp + 8], rax
    pop rax
    iretq
%endmacro


%macro syscallHandlerMaster 0
	pushStateWithoutRax
	push rdi
	mov rdi,rax
	call set_syscall
	pop rdi

	call sys_dispatcher

	popStateWithoutRax
	iretq
%endmacro

; %macro printReg 2
; 	mov rsi, %1
; 	lea rdi, [regsNames + 4 * %2]
; 	call [printRegPtr]			;Desreferencio el puntero a funcion
; %endmacro

; printAllRegs: 			;Imprime los registros de la instancia en la cual se lo llamó
; 	push rdi
; 	printReg rsi, 1
; 	pop rdi
; 	printReg rdi, 0
; 	printReg rax, 2
; 	printReg rbx, 3
; 	printReg rcx, 4
; 	printReg rdx, 5
; 	printReg r8, 6
; 	printReg r9, 7
; 	printReg r10, 8
; 	printReg r11, 9
; 	printReg r12, 10
; 	printReg r13, 11
; 	printReg r14, 12
; 	printReg r15, 13
; 	printReg rbp, 16
; 	ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

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
    mov rax, [rsp+15*8]
    mov [registers], rax

    mov byte [registersSaved], 1

    sti
%endmacro

%macro saveRegistersException 0
	cli
    mov [registersException+1*8], rax
    mov [registersException+2*8], rbx
    mov [registersException+3*8], rcx
    mov [registersException+4*8], rdx
    mov [registersException+5*8], rsi
    mov [registersException+6*8], rdi
    mov [registersException+7*8], rbp

    ; rsp
    mov rax, rsp
    add rax, 120
    mov [registersException+8*8], rax

    mov [registersException+9*8], r8
    mov [registersException+10*8], r9
    mov [registersException+11*8], r10
    mov [registersException+12*8], r11
    mov [registersException+13*8], r12
    mov [registersException+14*8], r13
    mov [registersException+15*8], r14
    mov [registersException+16*8], r15

    ; rip
    mov rax, [rsp+15*8]
    mov [registersException], rax
    sti
%endmacro

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Opcode Exception
_exception6Handler:
	exceptionHandler 6

;Software Interruption
_syscallHandler:
	syscallHandlerMaster

divzero:
    mov rax, 4
    xor rbx, rbx
    div rbx
    iretq

opcode:
    ud2
	saveRegisters 
	call inforegs
    ret
haltcpu:
	cli
	hlt
	ret

section .rodata
	; dd = 4 byte value. Hacemos un "array" donde cada posicion es de 4 bytes (cada caracter ocupa 1 byte, de esta forma todos terminan en 0)
	regsNames dd "rdi", "rsi", "rax", "rbx", "rcx", "rdx", "r8 ", "r9 ", "r10", "r11", "r12", "r13", "r14", "r15", "rip", "rsp", "rbp" ; 17 registros
SECTION .bss
    registersSaved resb 1
    aux resq 1
    registers resq 17
	registersException resq 17