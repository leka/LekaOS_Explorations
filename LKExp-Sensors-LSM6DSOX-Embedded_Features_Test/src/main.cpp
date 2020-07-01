/**
******************************************************************************
* @file    main.cpp
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Mbed integration of a LSM6DSOX MLC configuration 
******************************************************************************
*/

#include "mbed.h"

#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"

#include "LSM6DSOX_EmbeddedFeatures.h"

//#include "lsm6dsox_mlc.h"

using namespace EmbeddedFeatures;

//##################################################################################################
// Defines
//##################################################################################################

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT1 (A5)


#define DETECT_ACTIVITY 	1
#define DETECT_FREE_FALL 	0
#define DETECT_ORIENTATION 	0
#define DETECT_TAP 			0
#define DETECT_TILT 		0
#define DETECT_WAKE_UP 		1

//##################################################################################################
// Prototypes
//##################################################################################################

void ISR_INT1();

void activity();
void enableInterruptActivity();
void disableInterruptActivity();

void free_fall();
void enableInterruptFreeFall();
void disableInterruptFreeFall();

void orientation();
void enableInterruptOrientation();
void disableInterruptOrientation();

void single_double_tap();
void enableInterruptTap();
void disableInterruptTap();

void tilt();
void enableInterruptTilt();
void disableInterruptTilt();

void wake_up();
void enableInterruptWakeUp();
void disableInterruptWakeUp();

//##################################################################################################
// Global Variables
//##################################################################################################

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);

LSM6DSOX_EmbeddedFeatures lsm6dsox_embeddedFeatures(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);

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
	lsm6dsox_embeddedFeatures.getID(whoAmI);
	if (whoAmI != LSM6DSOX_ID)
	{
		printf("LSM6DSOX sensor is not connected properly\n");
		while(1);
	}
	printf("Device could be reached\n");
	
	// Restore default configuration
	printf("Restore default config\n");
	lsm6dsox_embeddedFeatures.restoreDefaultConfiguration();

	//-------------------------------------------------------------------------------------
	// Disabling I3C
	printf("Disable I3C\n");
	lsm6dsox_embeddedFeatures.disableI3C();		//disabling I3C on sensor
	INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)

	//-------------------------------------------------------------------------------------
	//Sensors init
	printf("Components init\n");
	lsm6dsox_embeddedFeatures.init();			//embedded features init
	lsm6dsox_accelerometer_component.init();	//xl init
	lsm6dsox_gyroscope_component.init();		//gy init

	//-------------------------------------------------------------------------------------
	// Setting up the xl and the gyro
	printf("Setting up xl and gyro\n");

	// Turn off Sensors
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	// Set full scale
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_4G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);

	// Set Output Data Rate.
	// Selected data rate have to be equal or greater than MLC's data rate.
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_accelerometer_component.setDataRate(417.0f);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setDataRate(417.0f);

	//-------------------------------------------------------------------------------------
	// Disable all embedded features interrupts
	printf("Disabling interrupts on all 6 features\n");
	disableInterruptActivity();
	disableInterruptFreeFall();
	disableInterruptOrientation();
	disableInterruptTap();
	disableInterruptWakeUp();
	disableInterruptTilt();
	//-------------------------------------------------------------------------------------
	// Setting up callbacks
	

	//-------------------------------------------------------------------------------------
	//Activating features
	activity();
	// free_fall();
	// orientation();
	// single_double_tap();
	// tilt();
	wake_up();

	enableInterruptActivity();
	enableInterruptWakeUp();

	//uint8_t activityPreviousStatus = 1;


	//-------------------------------------------------------------------------------------
	// Main loop
	printf("Starting while loop\n");
	while (1) {
		lsm6dsox_all_sources_t all_source;
		// Get status for all interrupts on INT1
		lsm6dsox_all_sources_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &all_source);
		

		printf("\nAll status : \n");
		printf("sleep_state  = %d \n", all_source.sleep_state);
		printf("free_fall  = %d \n", all_source.free_fall);
		printf("six_d  = %d \n", all_source.six_d);
		printf("double_tap  = %d \n", all_source.double_tap);
		printf("single_tap  = %d \n", all_source.single_tap);
		printf("wake_up  = %d \n", all_source.wake_up);

		// //ACTIVITY
		// //SLEEP
		// //if (all_source.sleep_state && DETECT_ACTIVITY == 1 && activityPreviousStatus == 1)
		// if (all_source.sleep_state)
		// {
		// 	printf("Inactivity Detected\n");
		// 	activityPreviousStatus = 0 ;
		// }

		// //WAKE UP
		// if (all_source.wake_up && DETECT_WAKE_UP  == 1 && activityPreviousStatus == 0)
		// {
		// 	printf("Wake-Up event on ");
		// 	if (all_source.wake_up_x)
		// 	printf("X");
		// 	if (all_source.wake_up_y)
		// 	printf("Y");
		// 	if (all_source.wake_up_z)
		// 	printf("Z");

		// 	printf(" direction\n");
		// }


		// //FREE FALL
		// if (all_source.free_fall && DETECT_FREE_FALL  == 1)
		// {
		// 	printf("Free Fall Detected\n");
		// }

		// //ORIENTATION
		// if (all_source.six_d && DETECT_ORIENTATION  == 1)
		// {
		// 	printf("6D Orientation switched to ");
		// 	if (all_source.six_d_xh)
		// 		printf("XH\n");
		// 	if (all_source.six_d_xl)
		// 		printf("XL\n");
		// 	if (all_source.six_d_yh)
		// 		printf("YH\n");
		// 	if (all_source.six_d_yl)
		// 		printf("YL\n");
		// 	if (all_source.six_d_zh)
		// 		printf("ZH\n");
		// 	if (all_source.six_d_zl)
		// 		printf("ZL\n");
		// }

		// //TAP DETECTION
		// //DOUBLE TAP
		// if (all_source.double_tap && DETECT_TAP == 1)
		// {
		// 	printf("D-Tap: ");
		// 	if (all_source.tap_x)
		// 	printf("x-axis\n");
		// 	else if (all_source.tap_y)
		// 	printf("y-axis\n");
		// 	else
		// 	printf("z-axis\n");
		// 	if (all_source.tap_sign)
		// 	printf("negative sign\n");
		// 	else
		// 	printf("positive sign\n");
		// }

		// //SINGLE TAP
		// if (all_source.single_tap && DETECT_TAP == 1)
		// {
		// 	printf("S-Tap: ");
		// 	if (all_source.tap_x)
		// 	printf("x-axis\n");
		// 	else if (all_source.tap_y)
		// 	printf("y-axis\n");
		// 	else
		// 	printf("z-axis\n");
		// 	if (all_source.tap_sign)
		// 	printf("negative sign\n");
		// 	else
		// 	printf("positive sign\n");
		// }

		// //TILT
		// uint8_t is_tilt;
		// /* Check if Tilt events */
		// lsm6dsox_tilt_flag_data_ready_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &is_tilt);
		// if (is_tilt && DETECT_TILT == 1)
		// {
		// 	printf("TILT Detected\n");
		// }

		
		ThisThread::sleep_for(500ms);
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



//------------------------------------------------------------------------------------------------------------------
// Activity
void activity(){
	/* Set duration for Activity detection to 9.62 ms (= 2 * 1 / ODR_XL) */
	lsm6dsox_wkup_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	/* Set duration for Inactivity detection to 4.92 s (= 2 * 512 / ODR_XL) */
	lsm6dsox_act_sleep_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	/* Set Activity/Inactivity threshold to 62.5 mg */
	lsm6dsox_wkup_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	/* Inactivity configuration: XL to 12.5 in LP, gyro to Power-Down */
	lsm6dsox_act_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_XL_12Hz5_GY_PD);
}

void enableInterruptActivity()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.sleep_change = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}
void disableInterruptActivity()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.sleep_change = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Free fall
void free_fall(){
	/* Set Free Fall duration to 3 and 6 samples event duration */
	lsm6dsox_ff_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x06);
	lsm6dsox_ff_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_FF_TSH_312mg);

}

void enableInterruptFreeFall()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}
void disableInterruptFreeFall()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Orientation
void orientation(){
	/* Set threshold to 60 degrees. */
	lsm6dsox_6d_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_DEG_60);

	/* LPF2 on 6D/4D function selection. */
	lsm6dsox_xl_lp2_on_6d_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);

	/*
	* To enable 4D mode uncomment next line.
	* 4D orientation detection disable Z-axis events.
	*/
	//lsm6dsox_4d_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
}

void enableInterruptOrientation()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.six_d = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptOrientation()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.six_d = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Tap
void single_double_tap(){
	/* Enable Tap detection on X, Y, Z */
	lsm6dsox_tap_detection_on_z_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_y_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_x_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);

	/* Set Tap threshold to 01000b, therefore the tap threshold
	* is 500 mg (= 12 * FS_XL / 32 )
	*/
	lsm6dsox_tap_threshold_x_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);
	lsm6dsox_tap_threshold_y_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);
	lsm6dsox_tap_threshold_z_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);

	/* Configure Single and Double Tap parameter
	*
	* For the maximum time between two consecutive detected taps, the DUR
	* field of the INT_DUR2 register is set to 0111b, therefore the Duration
	* time is 538.5 ms (= 7 * 32 * ODR_XL)
	*
	* The SHOCK field of the INT_DUR2 register is set to 11b, therefore
	* the Shock time is 57.36 ms (= 3 * 8 * ODR_XL)
	*
	* The QUIET field of the INT_DUR2 register is set to 11b, therefore
	* the Quiet time is 28.68 ms (= 3 * 4 * ODR_XL)
	*/
	lsm6dsox_tap_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x07);
	lsm6dsox_tap_quiet_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x03);
	lsm6dsox_tap_shock_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x03);

	/* Enable Single and Double Tap detection. */
	lsm6dsox_tap_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_BOTH_SINGLE_DOUBLE);
}

void enableInterruptTap()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.double_tap = PROPERTY_ENABLE;
	int1_route.single_tap = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptTap()
{
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.double_tap = PROPERTY_DISABLE;
	int1_route.single_tap = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Tilt
void tilt(){
	/* Enable Tilt in embedded function. */
	lsm6dsox_tilt_sens_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
}

void enableInterruptTilt()
{
	lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment if interrupt generation on Tilt INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);

	/* Uncomment to have interrupt latched */
	lsm6dsox_int_notification_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_ALL_INT_LATCHED);
}

void disableInterruptTilt()
{
	lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment if interrupt generation on Tilt INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Wake u
void wake_up(){
	/* Apply high-pass digital filter on Wake-Up function */
	lsm6dsox_xl_hp_path_internal_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_USE_SLOPE);

	/* Set Wake-Up threshold: 1 LSb corresponds to FS_XL/2^6 */
	lsm6dsox_wkup_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 2);
}

void enableInterruptWakeUp()
{
	lsm6dsox_pin_int1_route_t int1_route;
	/* Enable if interrupt generation on Wake-Up INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptWakeUp()
{
	lsm6dsox_pin_int1_route_t int1_route;
	/* Enable if interrupt generation on Wake-Up INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}