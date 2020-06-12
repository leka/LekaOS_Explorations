#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "LSM6DSOX_MachineLearningCore.h"
#include "mbed.h"

//#include "lsm6dsox_mlc.h"
#include "lsm6dsox_six_d_position.h"

using namespace MachineLearningCore;

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT1 (A5)
#define PIN_LSM6DSOX_INT2 (A4)

DigitalOut INT_1_LSM6DSOX(A5, 0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1); 
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
MachineLearningCore::LSM6DSOX_MachineLearningCore lsm6dsox_mlc(lsm6dsox_i2c, PIN_LSM6DSOX_INT1, PIN_LSM6DSOX_INT2);

int main(void) {
	/* Wait sensor boot time */
	ThisThread::sleep_for(10ms);
	printf("\nStarting a new run!\r\n\n");

	/* Disable I3C in order to use interrupts */
	lsm6dsox_mlc.disableI3C();

	// Set interrupt pins
	// delete(&INT_1_LSM6DSOX);
	DigitalIn INT_2_LSM6DSOX(A4);
	DigitalIn INT_1_LSM6DSOX(A5);

	//Inits
	lsm6dsox_mlc.init();
	lsm6dsox_accelerometer_component.init();
	lsm6dsox_gyroscope_component.init();

	//test of data rate
	float odr = 12.0f;
	lsm6dsox_mlc.setDataRate(odr);
	lsm6dsox_mlc.getDataRate(odr);

	printf("The data rate has been set to: %d (value was multiplied by 1000)\n", (int)(odr*1000));

	//Configuration of decision trees with the ucf from the lsm6dsox_six_d_position.h file
	lsm6dsox_mlc.setDecisionTrees(
		(MachineLearningCore::ucf_line_t *)lsm6dsox_six_d_position,
		(sizeof(lsm6dsox_six_d_position) / sizeof(MachineLearningCore::ucf_line_t)));

	

	/* At this point the device is ready to run but if you need you can also
	 * interact with the device but taking in account the MLC configuration.
	 *
	 * For more information about Machine Learning Core tool please refer
	 * to AN5259 "LSM6DSOX: Machine Learning Core".
	 */

	stmdev_ctx_t *ctx = lsm6dsox_mlc.TMP_getIoFunc();
	lsm6dsox_all_sources_t status;
	/* Turn off Sensors */
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(ctx, PROPERTY_ENABLE);


	/* Set full scale */
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_4G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);


	/* Route signals on interrupt pin 1 and 2 */
	lsm6dsox_mlc.enableTreeInterrupt(MachineLearningCoreTree::_TREE_1,
									 TreeInterruptNum::_INT1_AND_INT2);

	/* Configure interrupt pin mode notification */
	lsm6dsox_int_notification_set(ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

	/* Set Output Data Rate.
	 * Selected data rate have to be equal or greater with respect
	 * with MLC data rate.
	 */
	
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_accelerometer_component.setDataRate(26.0f);

	/* Main loop */
	while (1) {
		/* Read interrupt source registers in polling mode (no int) */
		lsm6dsox_all_sources_get(ctx, &status);
		if (status.mlc1) {

			// lsm6dsox_mlc_out_get(ctx, mlc_out);
			MachineLearningCoreData values;
			lsm6dsox_mlc.getData(values.data);
			// MLC0_SRC register values for 6D position
			// 0 = None
			// 1 = X-axis pointing up
			// 2 = X-axis pointing down
			// 3 = Y-axis pointing up
			// 4 = Y-axis pointing down
			// 5 = Z-axis pointing up
			// 6 = Z-axis pointing down

			switch (values.mlcTreeVal.tree1) {
				case 1:
					printf("X axis up\n");
					break;
				case 2:
					printf("X axis down\n");
					break;
				case 3:
					printf("Y axis up\n");
					break;
				case 4:
					printf("Y axis down\n");
					break;
				case 5:
					printf("Z axis up\n");
					break;
				case 6:
					printf("Z axis down\n");
					break;
				default:
					printf("None (data hasn't been updated by tree)\n");
					break;
			}
		}
	}

	// lsm6dsox_mlc();

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

//################################################################################################
// Main from Yann's example
// int main(void) {

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
// return 0;
// }
//################################################################################################