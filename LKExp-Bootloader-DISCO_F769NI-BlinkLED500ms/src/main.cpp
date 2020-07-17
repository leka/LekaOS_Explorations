#include "mbed.h"

DigitalOut led(LED1);

int main(void) {

	printf("\nStarting application: blinking LED 500 ms\n");

	while (true) {
		led = !led;
		rtos::ThisThread::sleep_for(500);
	}

	return 0;
}
