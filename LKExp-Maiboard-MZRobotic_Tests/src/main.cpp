#include "LekaBLE.h"
#include "LekaBluetooth.h"
#include "LekaBrightness.h"
#include "LekaIMU.h"
#include "LekaInterfaces.h"
#include "LekaMagnetometer.h"
#include "LekaMicrophone.h"
#include "LekaSD.h"
#include "LekaTemperature.h"
#include "LekaWifi.h"
#include "mbed.h"
#include "LekaMotors.h"
#include "LekaSpeaker.h"
#include "LekaFirmware.h"

LekaBrightness leka_brightness(brightness);
LekaMicrophone leka_microphone(microphone);
LekaIMU leka_imu(i2c1, PIN_INT1_IMU);
LekaMagnetometer leka_magnetometer(i2c1, PIN_DTRD_MAG);
LekaTemperature leka_temperature(i2c1, PIN_DRDY_TEMP);
LekaBLE leka_ble(spi5, ble_reset, NC);
LekaSD leka_sd(sd_on);
LekaWifi leka_wifi(PIN_ESP_ENABLE, PIN_RST_ESP);
LekaBluetooth leka_bluetooth(PC_6, PC_7, ble_reset); // try to inverse PC_6 and PC_7?
LekaSpeaker leka_speaker(sound_out, PIN_SON_ON);
LekaMotors leka_motors(motor_left, motor_right, PIN_IN1, PIN_IN2, PIN_IN3, PIN_IN4);
LekaFirmware leka_firmware(ce1_unselect, ce2_unselect, ce3_unselect);


int main(void) {
	printf("\nStarting a new run!\n\n");

	/** Tested (with return) **/
	// leka_brightness.runTest();
	// leka_microphone.runTest();
	// leka_imu.runTest();
	// leka_magnetometer.runTest();
	// leka_temperature.runTest();
	// leka_speaker.runTest();
	// leka_motors.runTest();
	// leka_ble.runTest();

	/** In progress **/
	
	/** On hold (functional) **/
	// leka_wifi.runTest();
	// leka_bluetooth.runTest();

	/** On hold **/
	leka_sd.runTest();
	// leka_firmware.runTest();

	printf("\nEnd of run!\n\n");
	return 0;
}
