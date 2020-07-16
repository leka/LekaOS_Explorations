// #include "LekaInterfaces.h"
#include "mbed.h"
// #include "LekaRFID.h"

// LekaRFID leka_rfid(PIN_RFID_TX, PIN_RFID_RX);

int main(void) {
	// printf("Starting a new run");
	// printf("");

	BufferedSerial pc(PA_9, PA_10, 115200);

	// Write array of char
	char tmp[] = "0123456789\n";
	pc.write(tmp, sizeof(tmp) - 1);

	// Write array of short
	uint8_t tab[] = {0x2A, 0x2B, 0x2C, 0x2D};
	ThisThread::sleep_for(1s);
	pc.write(tab, 4);
	pc.write("\n",1);

	// Read array of short
	uint8_t c;
	const uint8_t buffer_max_size = 10;
	uint8_t buffer[buffer_max_size] = {};
	uint8_t buffer_size = 0;
	
	ThisThread::sleep_for(10s);
	while(pc.readable() && buffer_size < buffer_max_size){
		pc.read(&c, 1);
		buffer[buffer_size] = c;
		buffer_size++;
	}
	pc.write(buffer, buffer_size);
	pc.write("\n",1);

	// Write (again) array of char
	// pc.write(tmp, sizeof(tmp) - 1);



	/** Tested (with return) **/
	/** In progress **/
	// leka_rfid.runTest();

	/** On hold **/

	ThisThread::sleep_for(1s);
	// printf("\nEnd of run!\n\n");
	return 0;
}
