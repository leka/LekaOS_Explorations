
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
void intSetup();

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
	//First message to serial
	printf("\nStarting a new run!\n\n");

	//-------------------------------------------------------------------------------------
	//Initialization
	ThisThread::sleep_for(10ms);	//waiting for sensor boot time
	
	//Test sensor connexion / check device ID
	uint8_t whoAmI;
	lsm6dsox_mlc.getID(whoAmI);
	if (whoAmI != LSM6DSOX_ID)
	{
		printf("LSM6DSOX sensor is not connected properly");
		while(1);
	}
	
	// Restore default configuration
	lsm6dsox_mlc.restoreDefaultConfiguration();

	//-------------------------------------------------------------------------------------
	// Disabling I3C 
	lsm6dsox_mlc.disableI3C();		//disabling I3C on sensor
	INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)

	//-------------------------------------------------------------------------------------
	//Sensors init
	lsm6dsox_mlc.init();						//mlc init
	lsm6dsox_accelerometer_component.init();	//xl init
	lsm6dsox_gyroscope_component.init();		//gy init

	//-------------------------------------------------------------------------------------
	// Writing a decision tree to the MLC

	// Configuration of decision trees with the ucf from the lsm6dsox_six_d_position.h file
	lsm6dsox_mlc.configureMLC( (MachineLearningCore::ucf_line_t *)lsm6dsox_six_d_position,
									(sizeof(lsm6dsox_six_d_position) / sizeof(MachineLearningCore::ucf_line_t)));

	//-------------------------------------------------------------------------------------
	//To manually set up the interrupts after the ucf config
	lsm6dsox_mlc.disableTreeInterrupt(Tree::_TREE_1, 
									  MachineLearningCore::TreeInterruptNum::_INT1_AND_INT2);
	lsm6dsox_mlc.allOnInt1Route(false);

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
	lsm6dsox_mlc.setInterruptBehavior(InterruptBehavior::_LATCHED);
	//lsm6dsox_int_notification_set(lsm6dsox_mlc.TMP_getIoFunc(), lsm6dsox_lir_t::LSM6DSOX_BASE_PULSED_EMB_LATCHED);
	
	// Route signals on interrupt
	lsm6dsox_mlc.enableTreeInterrupt(Tree::_TREE_1, TreeInterruptNum::_INT1);

	//-------------------------------------------------------------------------------------
	// Setting up callbacks
	lsm6dsox_mlc.attachInterrupt(ISR_INT1, InterruptNumber::_INT1);

	//emptying latched interrupts
	uint8_t status;
	lsm6dsox_mlc.getTreeInterruptStatus(Tree::_TREE_1, status);
	
	//-------------------------------------------------------------------------------------
	// Main loop
	while (1) {
		if(int1Flag)
		{
			uint8_t status;
			//check if the interrupt was on tree 1
			lsm6dsox_mlc.getTreeInterruptStatus(Tree::_TREE_1, status);
			if(status) printState6D();
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
	Data data;
	lsm6dsox_mlc.getData(data.array);
	// MLC0_SRC register values for 6D position
	// 0 = None
	// 1 = X-axis pointing up
	// 2 = X-axis pointing down
	// 3 = Y-axis pointing up
	// 4 = Y-axis pointing down
	// 5 = Z-axis pointing up
	// 6 = Z-axis pointing down

	switch (data.trees.tree1) {
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