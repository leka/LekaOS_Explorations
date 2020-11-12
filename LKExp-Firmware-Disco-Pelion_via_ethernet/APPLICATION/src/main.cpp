#include "mbed.h"

Ticker flipper;
DigitalOut led1(LED1, 1);
DigitalOut led2(LED2, 0);

/* USED FUNCTION */
int pelionOnlineExample();

/* MAIN PROGRAM */
void checkAlive()
{
	led1 = !led1;
	// led2 = !led2;
}

int main(void)
{
	printf("\nNew run!\n\n");

	flipper.attach(&checkAlive, 1.0);
	ThisThread::sleep_for(2s);

	pelionOnlineExample();

	while (true) { ThisThread::sleep_for(1s); }
	return 0;
}
