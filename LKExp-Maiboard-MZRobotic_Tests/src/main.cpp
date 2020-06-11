#include "LekaBrightness.h"
#include "LekaIMU.h"
#include "LekaInterfaces.h"
#include "LekaMicrophone.h"
#include "LekaMotors.h"
#include "LekaSpeaker.h"
#include "mbed.h"

LekaMicrophone leka_microphone(microphone);
LekaBrightness leka_brightness(brightness);
LekaIMU leka_imu(i2c1, PIN_INT1_IMU);
LekaSpeaker leka_speaker(sound_out);
LekaMotors leka_motors(motor_left, motor_right, PIN_IN1, PIN_IN2, PIN_IN3, PIN_IN4);

int main(void) {
	printf("\nStarting a new run!\r\n\n");

	leka_microphone.runTest();
	leka_brightness.runTest();
	leka_imu.runTest();
	leka_speaker.runTest();
	leka_motors.runTest();

	return 0;
}
