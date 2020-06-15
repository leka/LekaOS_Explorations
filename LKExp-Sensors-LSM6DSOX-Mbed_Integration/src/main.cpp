#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "LSM6DSOX_MachineLearningCore.h"
#include "mbed.h"

//#include "lsm6dsox_mlc.h"
#include "lsm6dsox_six_d_position.h"

using namespace MachineLearningCore;

//##################################################################################################
// Defines
//##################################################################################################

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
//#define PIN_LSM6DSOX_INT1 (A5)
#define PIN_LSM6DSOX_INT1 NC
#define PIN_LSM6DSOX_INT2 (A4)

//##################################################################################################
// Prototypes
//##################################################################################################

void ISR_btn();

void ISR_INT1();
void ISR_INT2();

void mlc_interrupt_on_1();
void mlc_interrupt_on_2();

//##################################################################################################
// Global Variables
//##################################################################################################

DigitalOut INT_1_LSM6DSOX(A5, 0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
MachineLearningCore::LSM6DSOX_MachineLearningCore lsm6dsox_mlc(lsm6dsox_i2c, PIN_LSM6DSOX_INT1, 
																			PIN_LSM6DSOX_INT2);

// Flags for interrupts
bool int1Flag = false;
bool int2Flag = false;

bool btnFlag = false;

//##################################################################################################
// Main
//##################################################################################################

int main(void) {

	//-------------------------------------------------------------------------------------
	// Initial setup

	/* Wait sensor boot time */
	ThisThread::sleep_for(10ms);
	printf("\nStarting a new run!\r\n\n");

	/* Disable I3C in order to use interrupts */
	lsm6dsox_mlc.disableI3C();

	//lsm6dsox_mlc.setInterrupt1Pin((A5));


	// Get sensor ID
	uint8_t id;
	lsm6dsox_mlc.getID(id);
	if (id != LSM6DSOX_ID) {
		printf("No id got from sensor\n\n");
		while (1)
			;
	}

	// Inits
	printf("Status init: %d\n", lsm6dsox_mlc.init());
	lsm6dsox_accelerometer_component.init();
	lsm6dsox_gyroscope_component.init();

	

	// Set interrupt pins
	// delete(&INT_1_LSM6DSOX);
	// DigitalIn INT_2_LSM6DSOX(A4);
	// DigitalIn INT_1_LSM6DSOX(A5);

	//-------------------------------------------------------------------------------------
	// MLC data rate test (write and read)

	// test of data rate
	float odr = 12.0f;
	lsm6dsox_mlc.setDataRate(odr);
	printf("Status data rate get: %d\n", lsm6dsox_mlc.getDataRate(odr));

	printf("The data rate has been set to: %d (value was multiplied by 1000)\n", (int)(odr * 1000));

	//-------------------------------------------------------------------------------------
	// Writing a decision tree to the MLC

	// Configuration of decision trees with the ucf from the lsm6dsox_six_d_position.h file
	lsm6dsox_mlc.setDecisionTrees(
		(MachineLearningCore::ucf_line_t *)lsm6dsox_six_d_position,
		(sizeof(lsm6dsox_six_d_position) / sizeof(MachineLearningCore::ucf_line_t)));

	//-------------------------------------------------------------------------------------
	// Setting up the xl and the gyro to work properly

	/* At this point the device is ready to run but if you need you can also
	 * interact with the device but taking in account the MLC configuration.
	 *
	 * For more information about Machine Learning Core tool please refer
	 * to AN5259 "LSM6DSOX: Machine Learning Core".
	 */

	/* Turn off Sensors */
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	/* Enable Block Data Update */
	lsm6dsox_block_data_update_set(lsm6dsox_mlc.TMP_getIoFunc(), PROPERTY_ENABLE);

	/* Set full scale */
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_4G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);

	/* Set Output Data Rate.
	 * Selected data rate have to be equal or greater with respect
	 * with MLC data rate.
	 */
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_accelerometer_component.setDataRate(26.0f);

	//-------------------------------------------------------------------------------------
	// Setting up interrupt for MLC

	/* Configure interrupt pin mode notification */
	lsm6dsox_int_notification_set(lsm6dsox_mlc.TMP_getIoFunc(), LSM6DSOX_BASE_PULSED_EMB_LATCHED);
	
	/* Route signals on interrupt pin 1 and 2 */
	lsm6dsox_mlc.enableTreeInterrupt(MachineLearningCoreTree::_TREE_1,
									 TreeInterruptNum::_INT1);


	lsm6dsox_pin_int1_route_t   pin_int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_mlc.TMP_getIoFunc(), &pin_int1_route);
	printf("Interrupt 1 enabled mlc1: %d\n", pin_int1_route.mlc1);



	//-------------------------------------------------------------------------------------
	// Testing interrupts
	//DigitalIn INT_1_LSM6DSOX(A5);
	InterruptIn int1(A5);
	int1.rise(ISR_INT1);
	// lsm6dsox_mlc.setInterrupt1Pin(A5);
	// lsm6dsox_mlc.attachInterrupt1(ISR_INT1);

	//lsm6dsox_mlc.attachInterrupt1(ISR_INT1);
	//lsm6dsox_mlc.attachInterrupt2(ISR_INT2);
	
	InterruptIn btn(BUTTON1);
	btn.rise(ISR_btn);

	/* Main loop */
	while (1) {

		if(int1Flag)
		{
			lsm6dsox_all_sources_t status;
			lsm6dsox_all_sources_get(lsm6dsox_mlc.TMP_getIoFunc(), &status);
			if (status.mlc1)
			{
				mlc_interrupt_on_1();
				printf("Interrupt on INT1\n\n");
			}
			int1Flag = false;
		}
		// lsm6dsox_all_sources_t status;
		// lsm6dsox_all_sources_get(lsm6dsox_mlc.TMP_getIoFunc(), &status);
		// if(status.mlc1)
		// {
		// 	printf("Interrupt detected from mlc1\n");
		// 	uint8_t intStatus;
		// 	lsm6dsox_mlc.readInterrupt2(intStatus);
		// 	printf("pin int 2 status: %d\n", intStatus);
		// }

		// uint8_t intStatus;
		// lsm6dsox_mlc.readInterrupt2(intStatus);
		// printf("pin int 2 status: %d\n", intStatus);

		ThisThread::sleep_for(1s);
	}

	return 0;
}

//##################################################################################################
// Functions
//##################################################################################################

void ISR_btn() { btnFlag = true; }

// ISR for INT1 (with flags)
void ISR_INT1() {
	int1Flag = true;
}

// ISR for INT2 (with flags)
void ISR_INT2() {
	lsm6dsox_all_sources_t status;
	lsm6dsox_all_sources_get(lsm6dsox_mlc.TMP_getIoFunc(), &status);
	if (status.mlc1) int2Flag = true;
}

void mlc_interrupt_on_1() {

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

void mlc_interrupt_on_2() {

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

//################################################################################################
// Blink in case of real problems

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

//################################################################################################
// Main from Yann's example

// #define PIN_I2C1_SDA (D14)
// #define PIN_I2C1_SCL (D15)
// //#define PIN_LSM6DSOX_INT1 (A5)
// #define PIN_LSM6DSOX_INT1 NC

// DigitalOut INT_1_LSM6DSOX(A5, 0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

// I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
// Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

// Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c,
// PIN_LSM6DSOX_INT1); Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c,
// PIN_LSM6DSOX_INT1);

// int main(void) {

// 	lsm6dsox_gyroscope_component.init();
// 	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);
// 	lsm6dsox_gyroscope_component.setDataRate(104.0f);
// 	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_125DPS);

// 	lsm6dsox_accelerometer_component.init();
// 	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
// 	lsm6dsox_accelerometer_component.setDataRate(104.0f);
// 	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_2G);

// 	Component::GyroscopeData gy;
// 	Component::AccelerometerData ac;

// 	while (1) {
// 		lsm6dsox_gyroscope_component.getData(gy.data);
// 		printf("%d %d %d \n", (int)gy.mdps.x, (int)gy.mdps.y, (int)gy.mdps.z);
// 		lsm6dsox_accelerometer_component.getData(ac.data);
// 		printf("%d %d %d \n", (int)ac.mg.x, (int)ac.mg.y, (int)ac.mg.z);

// 		printf("\n");
// 		ThisThread::sleep_for(300ms);
// 	}
// 	return 0;
// }
//################################################################################################