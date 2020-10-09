// #include "QspiMemory.h"
// #include "drivers/QSPI.h"
#include "Updater.h"
#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

#if !DEVICE_QSPI
#error[NOT_SUPPORTED] QSPI not supported for this target
#endif

#define APPLICATION_ADDR 0x08020000

int main(void)
{
	printf("Hello, I'm the bootloader!\n");

	printf("Checking if a new application is available...\n");

	ThisThread::sleep_for(5s);
	if (Updater::checkNew()) {
		printf("New application available!\n\n");

		printf("Now I'm written the application to the MCU...\n");
		Updater::flashQSPI(APPLICATION_ADDR);
		printf("Application written to MCU.\n\n");

		printf("Starting new application now!\n");

	} else {
		printf("No application available.\n\n");

		printf("Starting current application now!\n");
	}
	wait_us(2000);
	mbed_start_application(POST_APPLICATION_ADDR);

	return 0;
}
