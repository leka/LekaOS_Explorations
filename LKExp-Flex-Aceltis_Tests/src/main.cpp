#include "LekaInterfaces.h"
#include "LekaLEDs.h"
#include "LekaRFID.h"
#include "mbed.h"
#include "LekaTouch.h"

LekaRFID leka_rfid(PIN_RFID_TX, PIN_RFID_RX);
LekaLEDs leka_leds(leds_ears, leds_belt);
LekaTouch leka_touch(i2c3, mux_reset, mux_inta, mux_intb);

int i2cConnected()
{
	printf("Test on I2C...\n");
	// I2C i2c1(PB_9, PB_8); //I2C on mainboard, in comparison
	I2C i2c3(PC_9, PH_7);

	char cmd[1];
	int addr8bit	 = 0x00;
	int count_device = 0;

	for (int i = 0; i < 256; i++) {
		cmd[0] = 0X0F;
		i2c3.write(addr8bit, cmd, 1);
		thread_sleep_for(1);
		i2c3.read(addr8bit, cmd, 1);
		if (cmd[0] != 0xF) {
			printf("Address 0x%X : 0x%X\r\n", addr8bit, cmd[0]);
			addr8bit += 1;	 //(Optional) Avoid duplication
			count_device++;
		}
		addr8bit += 1;
	}
	printf("END I2C\r\n");
	if (count_device == 0) { return -1; }
	return 0;
}

int main(void)
{
	printf("Starting a new run\n");

	/*  Test on I2C (see if flex connected)  */
	if (i2cConnected() != 0) {
		printf("Flex is not connected.\n");
		return -1;
	}
	ThisThread::sleep_for(3s);

	/** Tested (with return) **/
	// leka_leds.runTest();
	// leka_touch.runTest(100);

	/** In progress **/
	
	// leka_rfid.runTest();

	/** On hold **/
	

	printf("\nEnd of run!\n\n");
	return 0;
}
