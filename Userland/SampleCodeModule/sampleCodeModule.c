// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include <shell.h>
#include <tests.h>
#include <syscallslib.h>
char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
	//Aca se podrian correr los test
	clear();
	printf("Welcome to capOs. Run HELP command to see the available programs!\n");
	shell();
	// sys_process(&shell,9,0,NULL,NULL,NULL,0);
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}