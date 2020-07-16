#include "LekaRFID.h"

// static UnbufferedSerial interface(PIN_RFID_TX, PIN_RFID_RX, 57600);
BufferedSerial pc(PA_9, PA_10, 115200);

namespace LekaRFIDNS {
	volatile int _time_counter = 0;
	void counter() { _time_counter++; }
}	// namespace LekaRFIDNS

LekaRFID::LekaRFID(PinName uart_tx, PinName uart_rx) : _interface(uart_tx, uart_rx, _baud) {}

void LekaRFID::rfidWrite(uint8_t cmd[], uint8_t len) {
    pc.write(cmd, len);
    ThisThread::sleep_for(1s);
}

void LekaRFID::rfidRead() {
	uint8_t c;
    char tmp[] = "Waiting\n";
	bool all_read = false;
	int i		  = 0;
	while (false == all_read) {
		if (pc.readable()) {
			pc.read(&c, 1);
			// printf("printf:%x\n", c);
            pc.write(&c,1);
		} else {
			// printf("printf:Waiting to read... %d\n", i);
            pc.write(tmp, 3);
			i++;
			ThisThread::sleep_for(1s);
		}
		if (i > 30) { all_read = true; }
        pc.sync();
	}
}

void LekaRFID::init() {
	// printf("CR95HF_Init\r\n");

	uint8_t commandInfo[]		  = {0x01, 0x00};
	// printf("Command created\n");
	// rfidWrite(commandInfo, 2);
    pc.write(commandInfo, 2);
	wait_us(10000);
    ThisThread::sleep_for(1s);
	rfidRead();
}

void LekaRFID::runTest(int duration_sec) {
	// printf("\nTest of RFID reader!\n");
	init();

	// LekaRFIDNS::_time_counter = 0;
	// _ticker.attach(&LekaRFIDNS::counter, 1.0);

	// while (LekaRFIDNS::_time_counter < duration_sec) { wait_us(1000000); }

	printf("\n");
}
