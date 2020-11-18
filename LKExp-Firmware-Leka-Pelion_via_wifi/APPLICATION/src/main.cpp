#include "mbed.h"

Ticker flipper;
DigitalOut led1(LED1, 1);
DigitalOut led2(LED2, 0);
DigitalOut led3(LED2, 1);

/* USED FUNCTION */
int pelionOnlineExample();
int ethernetExample();
int qspiExample();

/* MAIN PROGRAM */
void checkAlive()
{
	led1 = !led1;
	led2 = !led2;
	led3 = !led3;
}

int main(void)
{
	printf("\nNew run!\n\n");

	flipper.attach(&checkAlive, 1.0);
	ThisThread::sleep_for(2s);

	// ethernetExample();
	// qspiExample();
	pelionOnlineExample();

	while (true) {
		ThisThread::sleep_for(3s);
		led3 = !led3;
	}
	return 0;
}
