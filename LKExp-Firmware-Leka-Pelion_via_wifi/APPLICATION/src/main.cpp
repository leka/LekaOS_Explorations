#include "mbed.h"

/* USED FUNCTION */
int pelionOnlineExample();

#include "LekaWifi.h"
Wifi leka_wifi;

/* MAIN PROGRAM */

int main(void)
{
	printf("\nNew run!\n\n");
	printf("This is a success!\n");

	ThisThread::sleep_for(2s);

	// leka_wifi.start();
	pelionOnlineExample();

	while (true) { ThisThread::sleep_for(1s); }
	return 0;
}
