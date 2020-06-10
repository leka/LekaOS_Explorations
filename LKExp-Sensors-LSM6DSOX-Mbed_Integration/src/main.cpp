#include "mbed.h"

#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"


#include "lsm6dsox_mlc.h"



// #define PIN_I2C1_SDA (D14)
// #define PIN_I2C1_SCL (D15)
// #define PIN_LSM6DSOX_INT NC

// DigitalOut INT_1_LSM6DSOX(A5,
// 						  0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

// I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
// Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);
// Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);
// Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);

int main(void) {
	printf("\nStarting a new run!\r\n\n");

	// lsm6dsox_gyroscope_component.init();
	// lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);
	// lsm6dsox_gyroscope_component.setDataRate(104.0f);
	// lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_125DPS);
	
	// lsm6dsox_accelerometer_component.init();
	// lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	// lsm6dsox_accelerometer_component.setDataRate(104.0f);
	// lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_2G);

	// Component::GyroscopeData gy;
	// Component::AccelerometerData ac;


	// while (1) {
	// 	lsm6dsox_gyroscope_component.getData(gy.data);
	// 	printf("%d %d %d \n\r", (int)gy.mdps.x, (int)gy.mdps.y, (int)gy.mdps.z);
	// 	lsm6dsox_accelerometer_component.getData(ac.data);
	// 	printf("%d %d %d \n\r", (int)ac.mg.x, (int)ac.mg.y, (int)ac.mg.z);
	// 	ThisThread::sleep_for(300);
	// }

	lsm6dsox_mlc();

	return 0;
}


// //Connection test
// int main(void) {

// 	DigitalOut led(LED1);

// 	while (true) {
//         led = !led;
// 		static int i = 0;
// 		printf("%d\n", i);
// 		++i;
// 		ThisThread::sleep_for(1s);
// 	}

// 	return 0;
// }

