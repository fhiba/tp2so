#include <stdlib.h>
#include <syscalls.h>
#include <lib.h>
#include <naiveConsole.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

static void zero_division();
static void invalid_opcode();

static const char* registerNames[17] = {
    "RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"
};

extern uint64_t registersException[17];

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	if(exception == INVALID_OPCODE_EXCEPTION_ID)
		invalid_opcode();
}

static void zero_division() {
	// saveRegisters();
	write(1, "Err: You may not divide by zero.\n", 33);
	 for(int i = 0;i<17;i++){
		write(4,(char*)registerNames[i],3);
		write(1,": ",2);
		ncPrintBase(registersException[i],16);
		ncNewline();
	}
	sleep(3000);
}

static void invalid_opcode() {
	write(1, "Err: Invalid Opcode\n", 20);
	for(int i = 0;i<17;i++){
		write(4,(char*)registerNames[i],3);
		write(1,": ",2);
		ncPrintBase(registersException[i],16);
		ncNewline();
	}
}