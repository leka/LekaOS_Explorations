#include "mbed.h"

BufferedSerial pc(USBTX,USBRX);
Timer timer;

int main(void) {
	pc.sync();
	printf("File reception program\n");
	
	char* message = new char;
	unsigned long i=0;
	
	printf("Waiting for file from USB\n");
	while(!pc.readable());
	
	printf("Receiving file...\n");
	timer.start();
	while(timer.read_ms()<1000) {
		if(pc.readable()) {
			timer.reset();
			pc.read(message+i,1);
			i++;
		}
	}
	timer.stop();
	pc.write(message,i);
	delete(message);
	
	rtos::ThisThread::sleep_for(1000);
	printf("\nCharacters received: %ld\n",i);

	return 0;
}
