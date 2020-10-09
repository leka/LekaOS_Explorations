#include "PinNames.h"
#include "Updater.h"
#include "mbed.h"

// DigitalOut led(LED2, 0); // Use LED to visualy confirm the flash of the new application
BufferedSerial serial(PA_9, PA_10, 115200);

int main(void)
{
	bool update_data_available = false;
	uint8_t buffer[0x2]		   = {0};

	printf("Hello, I'm application!\n");

	int counter = 0;
	printf("I'm running in a loop waiting for an new application to be sent to me...\n");
	update_data_available = serial.readable();
	while (!update_data_available) {
		counter++;
		printf("%d\n", counter);
		ThisThread::sleep_for(1s);
		// fflush(stdout);
		update_data_available = serial.readable();
		// serial.read(buffer, 0x01);
		printf("Available : %d  value: %X   ", update_data_available, buffer[0]);
	}
	printf("\n\n");

	printf("An application is available! I'm writing the application to QSPI flash!\n\n");
	/* Program all */
	int err = Updater::receiveFile();

	ThisThread::sleep_for(5s);
	if (err != 0) {
		printf("An error occurs : %d\n", err);
	} else {
		printf("Application written to QSPI!\n");
	}

	printf("Rebooting...\n\n");
	// wait_us(2000);
	ThisThread::sleep_for(2ms);
	// mbed_start_application(POST_APPLICATION_ADDR);
	system_reset();
	return 0;
}
