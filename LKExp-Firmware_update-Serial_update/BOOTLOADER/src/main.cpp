#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

int main(void)
{
	printf("Hello, I'm the bootloader!\n");
	// printf("Starting bootloader\nLaunching application at address 0x%x \n",POST_APPLICATION_ADDR);
	printf("Waiting for 5 seconds...");
	fflush(stdout);
	for (int i = 1 ; i < 6 ;i++) {
		ThisThread::sleep_for(1s);
		printf(" %d...", i);
		fflush(stdout);
	}
	printf("\nStarting application now!\n\n\n");
	wait_us(2000);
	mbed_start_application(POST_APPLICATION_ADDR);
	return 0;
}
