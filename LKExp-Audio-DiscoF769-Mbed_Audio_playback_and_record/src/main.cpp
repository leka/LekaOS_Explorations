#include "mbed.h"

int main(void) {

	while (true) {
		printf("Hello, World!\n");
		rtos::ThisThread::sleep_for(1000);
	}

	return 0;
}
