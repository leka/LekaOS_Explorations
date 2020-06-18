
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
#define PIN_LSM6DSOX_INT1 (A5)
#define PIN_LSM6DSOX_INT2 (A4)

//##################################################################################################
// Prototypes
//##################################################################################################

void ISR_INT1();
void printState6D();

//##################################################################################################
// Global Variables
//##################################################################################################

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
MachineLearningCore::LSM6DSOX_MachineLearningCore lsm6dsox_mlc(lsm6dsox_i2c, PIN_LSM6DSOX_INT1, 
																			PIN_LSM6DSOX_INT2);


//The A5 (INT1) pin has to be set to low at startup in order for the LSM6DSOX to mantain I2C connectivity
//Once the sensor has had time to startup, it can receive the "disable I3C" command, thus forcing I2C mode
//Once I3C has been deactivated, INT1 pin has to be set to input for the interrupts to work properly
DigitalInOut INT_1_LSM6DSOX(PIN_LSM6DSOX_INT1, PIN_OUTPUT, PullNone, 0);


// Flags for interrupts
bool int1Flag = false;

//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	//-------------------------------------------------------------------------------------
	// Initial setup

	ThisThread::sleep_for(10ms);	//waiting for sensor startup
	lsm6dsox_mlc.disableI3C();		//disabling I3C on sensor
	INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)

	//First message to serial
	printf("\nStarting a new run!\n\n");

	// Inits
	lsm6dsox_mlc.init();						//mlc
	lsm6dsox_accelerometer_component.init();	//xl
	lsm6dsox_gyroscope_component.init();		//gy

	//-------------------------------------------------------------------------------------
	// Writing a decision tree to the MLC

	// Configuration of decision trees with the ucf from the lsm6dsox_six_d_position.h file
	lsm6dsox_mlc.setDecisionTrees(
		(MachineLearningCore::ucf_line_t *)lsm6dsox_six_d_position,
		(sizeof(lsm6dsox_six_d_position) / sizeof(MachineLearningCore::ucf_line_t)));

	//-------------------------------------------------------------------------------------
	// //state of interrupts before setup
	// lsm6dsox_pin_int1_route_t   pin_int1_route;
	// lsm6dsox_pin_int2_route_t   pin_int2_route;

	// //disable int1 and int2
	// lsm6dsox_pin_int1_route_get(lsm6dsox_mlc.TMP_getIoFunc(), &pin_int1_route);
	// lsm6dsox_pin_int2_route_get(lsm6dsox_mlc.TMP_getIoFunc(), NULL, &pin_int2_route);
	// pin_int1_route.mlc1 = PROPERTY_DISABLE;
	// pin_int2_route.mlc1 = PROPERTY_DISABLE;
	// lsm6dsox_pin_int1_route_set(lsm6dsox_mlc.TMP_getIoFunc(), pin_int1_route);
	// lsm6dsox_pin_int2_route_set(lsm6dsox_mlc.TMP_getIoFunc(), NULL, pin_int2_route);

	// //disable rerouting of int2 on int1
	// lsm6dsox_all_on_int1_set(lsm6dsox_mlc.TMP_getIoFunc(), PROPERTY_DISABLE);
	

	//-------------------------------------------------------------------------------------
	// Setting up the xl and the gyro to work properly
	// TODO
	// we have to work out where to do these verifications
	
	// Turn off Sensors
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	// Enable Block Data Update
	// TODO
	// this still has to be clarified
	lsm6dsox_block_data_update_set(lsm6dsox_mlc.TMP_getIoFunc(), PROPERTY_ENABLE);

	// Set full scale
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_4G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);

	// Set Output Data Rate.
	// Selected data rate have to be equal or greater with respect
	// with MLC data rate.
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_accelerometer_component.setDataRate(26.0f);

	//-------------------------------------------------------------------------------------
	// Setting up interrupt for MLC

	// Configure interrupt pin mode notification
	// TODO
	// this still has to be clarified
	lsm6dsox_int_notification_set(lsm6dsox_mlc.TMP_getIoFunc(), LSM6DSOX_ALL_INT_LATCHED); 
	//   LSM6DSOX_ALL_INT_PULSED            = 0
	//   LSM6DSOX_BASE_LATCHED_EMB_PULSED   = 1
	//   LSM6DSOX_BASE_PULSED_EMB_LATCHED   = 2
	//   LSM6DSOX_ALL_INT_LATCHED           = 3
	
	//--------------------------------------------------------------
	//int setup
	// Route signals on interrupt
	lsm6dsox_mlc.enableTreeInterrupt(MachineLearningCoreTree::_TREE_1, TreeInterruptNum::_INT1);

	//-------------------------------------------------------------------------------------
	// Setting up callbacks
	lsm6dsox_mlc.attachInterrupt(ISR_INT1, InterruptNumber::_INT1);

	//emptying latched interrupts
	lsm6dsox_all_sources_t status;
	lsm6dsox_all_sources_get(lsm6dsox_mlc.TMP_getIoFunc(), &status);
	
	//-------------------------------------------------------------------------------------
	// Main loop
	while (1) {
		if(int1Flag)
		{
			lsm6dsox_all_sources_t status;
			lsm6dsox_all_sources_get(lsm6dsox_mlc.TMP_getIoFunc(), &status);
			
			if (status.mlc1) printState6D();	
			int1Flag = false;
		}
		ThisThread::sleep_for(200ms);
	}
	return 0;
}

//##################################################################################################
// Functions
//##################################################################################################

/**
 * @brief ISR for INT1
 * 
 */
void ISR_INT1() {
	int1Flag = true;
}

/**
 * @brief Printing state of 6D position
 * 
 */
void printState6D()
{
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
		case 0:
			printf("None (Uncertain position)\n");
			break;
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
	}
}

//################################################################################################
// Blink (in case of real problems)

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