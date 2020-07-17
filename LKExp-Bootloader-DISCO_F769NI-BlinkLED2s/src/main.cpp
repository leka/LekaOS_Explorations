#include "mbed.h"

DigitalOut led(LED1);

int main(void) {

	printf("\nStarting application: blinking LED 2 s\n");

	while (true) {
		led = !led;
		rtos::ThisThread::sleep_for(2000);
	}

	return 0;
}
