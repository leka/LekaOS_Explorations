#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

#define BLINK_500MS_APP_ADDR 0x8040000
#define BLINK_2S_APP_ADDR 0x8080000
#define BLINK_APP_SIZE 0x10000

FlashIAP flash;

int main(void) {
	
	printf("Starting bootloader...\n");
	
	flash.init();
	
	char* buffer = new char[BLINK_APP_SIZE];
	uint32_t addr = BLINK_500MS_APP_ADDR;
	
	printf("Copying program from address 0x%x \n", addr);
	
	if(flash.read(buffer,addr,BLINK_APP_SIZE))
		printf("Unable to read\n");
	else {
		if(flash.program(buffer,POST_APPLICATION_ADDR,BLINK_APP_SIZE))
			printf("Unable to write\n");
		else {
			printf("Launching program at address 0x%x, copied from 0x%x \n", POST_APPLICATION_ADDR, addr);
			mbed_start_application(POST_APPLICATION_ADDR);
		}
	}

	return 0;
}
