#include "LekaRFID.h"

BufferedSerial interface(PIN_RFID_TX, PIN_RFID_RX, 57600);

// namespace LekaRFIDNS {
// 	volatile int _time_counter = 0;
// 	void counter() { _time_counter++; }
// }	// namespace LekaRFIDNS

LekaRFID::LekaRFID(PinName uart_tx, PinName uart_rx) : _interface(uart_tx, uart_rx, _baud) {}

void LekaRFID::init()
{
	printf("CR95HF_Init\r\n");

	uint8_t rfid_properties_command[] = {0x00, 0x01};
	uint8_t *buff					  = new uint8_t[1];

	printf("Expected response : 00 0F 4E 46 43 20 46 53 32 4A 41 53 54 34 00 2A CE\n");
	ThisThread::sleep_for(1s);

	while (1) {
		for (int i = 0; i < sizeof(buff); i++) {
			printf("%X ", buff[i]);
			buff[i] = 0;
		}
		printf("\n");
		wait_us(1000000);
		interface.write(rfid_properties_command, sizeof(rfid_properties_command));
		if (interface.readable()) { interface.read(buff, (size_t)sizeof(buff)); }
	}

	return;
}

void LekaRFID::initBis()
{
	uint8_t *buff	= new uint8_t[1];
	uint8_t start[] = {0x00, 0x01};
	uint8_t echo[]	= {0x55};
	_interface.write(buff, 2);

	for (int i = 0; i < 1000; i++) {
		for (int i = 0; i < sizeof(buff); i++) {
			printf("%x ", buff[i]);
			buff[i] = 0;
		}
		printf("\n");
		wait_us(1000000);
		_interface.write(start, sizeof(start));
		if (_interface.readable()) { _interface.read(buff, (size_t)sizeof(buff)); }
	}
	return;
}

void LekaRFID::runTest(int duration_sec)
{
	printf("\nTest of RFID reader!\n");
	ThisThread::sleep_for(1s);
	// init();
	initBis();

	// LekaRFIDNS::_time_counter = 0;
	// _ticker.attach(&LekaRFIDNS::counter, 1.0);

	// while (LekaRFIDNS::_time_counter < duration_sec) { wait_us(1000000); }

	printf("\n");
}
