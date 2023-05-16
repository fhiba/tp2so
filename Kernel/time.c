#include <time.h>
#include <interrupts.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void tsleep(long ms){
	long final = ticks + ms/55;
	while(ticks <= final)
		_hlt();
}