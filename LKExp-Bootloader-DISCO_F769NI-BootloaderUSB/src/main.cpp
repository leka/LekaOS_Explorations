#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

BufferedSerial pc(USBTX,USBRX);
Timer timer;
FlashIAP flash;

int main(void) {
	
	printf("Starting bootloader\n");
	
	flash.init();
	pc.sync();
	
	char* newapp = new char;
	unsigned long size = 0;
	
	printf("\nWaiting for new app from USB\n");
	while(!pc.readable());
	
	printf("Receiving app...\n");
	timer.start();
	while(timer.read_ms()<1000) {
		if(pc.readable()) {
			timer.reset();
			pc.read(newapp+size,1);
			size++;
		}
	}
	timer.stop();
	
	printf("App received (size: %ld bytes), flashing...\n", size);
	if(flash.program(newapp,POST_APPLICATION_ADDR,size))
		printf("Unable to write in flash\n");
	
	else {
		printf("Launching new app at address 0x%x, received via USB\n", POST_APPLICATION_ADDR);
		mbed_start_application(POST_APPLICATION_ADDR);
	}
	
	return 0;
}