#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

int main(void) {

	printf("Starting bootloader\nLaunching application at address 0x%x \n",POST_APPLICATION_ADDR);
	mbed_start_application(POST_APPLICATION_ADDR);

	return 0;
}
