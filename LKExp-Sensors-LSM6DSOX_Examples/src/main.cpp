#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "mbed.h"

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT NC

DigitalOut INT_1_LSM6DSOX(PF_9,
						  0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);
Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);

int main(void) {
	printf("\nStarting a new run!\r\n\n");

	lsm6dsox_gyroscope_component.init();
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setDataRate(104.0f);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_125DPS);

	Component::GyroscopeData gy;
	while (1) {
		lsm6dsox_gyroscope_component.getData(gy.data);
		printf("%d %d %d \n", (int)gy.mdps.x, (int)gy.mdps.y, (int)gy.mdps.z);
		ThisThread::sleep_for(1s);
	}

	return 0;
}
