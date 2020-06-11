#include "mbed.h"

DigitalOut led(LED2);

// basic hello woorld program with mbed on F769NI Disco
int main(void) {
	HAL_Init();
	
	while (true) {
	
		printf("Hello, World!");
		led = !led;
		rtos::ThisThread::sleep_for(1000);
	}

	return 0;
}
