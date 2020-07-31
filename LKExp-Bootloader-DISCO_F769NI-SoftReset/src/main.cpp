#include "mbed.h"

int main(void) {

	printf("Simple reboot app\nRebooting in:\n");
	for (int i=5;i>0;i--) {
		printf("%d...\n",i);
		rtos::ThisThread::sleep_for(1000);
	}
	NVIC_SystemReset();

	return 0;
}
