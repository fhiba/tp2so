/* sampleCodeModule.c */
#include <shell.h>
#include <tests.h>

char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
	//Aca se podrian correr los test
	clear();
	printf("Welcome to capOs. Run HELP command to see the available programs!\n");
	shell();
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}