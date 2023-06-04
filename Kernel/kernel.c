#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <keyboard_driver.h>
#include <idtLoader.h>
#include <syscalls.h>
#include <speakerDriver.h>
#include <mmu_wrapper.h>
#include <scheduler.h>
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void *const heap_address = (void *)0x600000;
static void *const mmu_address = (void *)0x50000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];


	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);


	clearBSS(&bss, &endOfKernel - &bss);


	return getStackBase();
}
int main()
{	
	new_mmu(mmu_address,heap_address);
	startPos();
	ncPrint("hola");
	//beep();
	initScheduler();

	char argv[ARG_QTY][ARG_LEN];
	argv[0][0] = 'S';
	argv[0][1] = 'h';
	argv[0][2] = 'e';
	argv[0][3] = 'l';
	argv[0][4] = 'l';
	argv[0][5] = 0;
  	create_process((uint64_t)sampleCodeModuleAddress, 1, 1, argv, NULL, NULL);
	load_idt();
	return 0;
}


