/**
******************************************************************************
 * @file    main.cpp
 * @author  Maxime Blanc and Samuel Hadjes
 * @brief   LSM6DSOX embedded features test
 * 
 * This programm allows to test 6 of the embedded features of the LSM6DSOX sensor
 * These are the features :
 * 	- Wake Up 	: detect when the device starts to be moved
 * 	- Sleep 	: detect if the device has been inactive for a given time
 * 	- Free Fall : detect if the device has been in free fall
 * 	- 6D-4D		: detect what axis of the device is pointing up
 * 	- Tap		: detect single or double taps on the device
 * 	- Tilt 		: detect if the device has been tilted
 * 
 * All these features generate an interrupt when they have been activated and they sense a change of state.
 * The program checks the interrupt flags for all features and tells which is the one having changed
 * 
 * To configure what feature is tested, the DETECT_<FEATURE_NAME> macro for the feature has to be set to 1
 * 
 * Some info about the setup of each feature is given in the documentation of the corresponding function
 * 
 * ST samples can be found here : https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsox_STdC/example
 * More info about the LSM6DSOX can be found here : https://github.com/leka/LekaOS_Explorations
******************************************************************************
*/

//##################################################################################################
// Includes
//##################################################################################################

#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "LSM6DSOX_EmbeddedFeatures.h"
#include "mbed.h"

using namespace EmbeddedFeatures;

//##################################################################################################
// Defines
//##################################################################################################

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT1 (A5)

#define DETECT_WAKE_UP		0
#define DETECT_SLEEP		0
#define DETECT_FREE_FALL	0
#define DETECT_6D_4D		0
#define DETECT_TAP			0
#define DETECT_TILT			0

//##################################################################################################
// Prototypes
//##################################################################################################

void setup_sleep_state();
void enableInterruptSleepState();
void disableInterruptSleepState();

void setup_free_fall();
void enableInterruptFreeFall();
void disableInterruptFreeFall();

void setup_orientation();
void enableInterruptOrientation();
void disableInterruptOrientation();

void setup_single_double_tap();
void enableInterruptTap();
void disableInterruptTap();

void setup_tilt();
void enableInterruptTilt();
void disableInterruptTilt();

void setup_wake_up();
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

// The A5 (INT1) pin has to be set to low at startup in order for the LSM6DSOX to mantain I2C
// connectivity Once the sensor has had time to startup, it can receive the "disable I3C" command,
// thus forcing I2C mode Once I3C has been deactivated, INT1 pin has to be set to input for the
// interrupts to work properly
DigitalInOut INT_1_LSM6DSOX(PIN_LSM6DSOX_INT1, PIN_OUTPUT, PullNone, 0);

//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	//-------------------------------------------------------------------------------------
	// Booting sensor and checking connection

	// First message to serial
	printf("\n----------------------------------\n");
	printf("Starting a new run!\n\n");

	// Initialization
	ThisThread::sleep_for(10ms);   // waiting for sensor boot time

	// Test sensor connexion / check device ID
	uint8_t whoAmI;
	lsm6dsox_embeddedFeatures.getID(whoAmI);
	if (whoAmI != LSM6DSOX_ID) {
		printf("LSM6DSOX sensor is not connected properly\n");
		while (1)
			;
	}
	printf("LSM6DSOX detected\n");

	//-------------------------------------------------------------------------------------
	// Check that at least one feature is activated
	if(!(DETECT_WAKE_UP || DETECT_SLEEP || DETECT_FREE_FALL || DETECT_6D_4D || DETECT_TAP || DETECT_TILT))
	{
		printf("\n\nThe feature you want to test must be enabled via the macros at start of code\n");
		printf("Program stopped\n");
		while(1);
	}

	//-------------------------------------------------------------------------------------
	// Restoring default state and enforcing I2C comm (altought INT1 pin is not used)

	// Restore default configuration (always start with the same settings)
	lsm6dsox_embeddedFeatures.restoreDefaultConfiguration();

	// Disabling I3C (this allows to use INT1 pin as input without loosing I2C)
	lsm6dsox_embeddedFeatures.disableI3C();	  // disabling I3C on sensor
	INT_1_LSM6DSOX.input();	  // setting up INT1 pin as input (for interrupts to work)

	//-------------------------------------------------------------------------------------
	// Components initialization

	lsm6dsox_accelerometer_component.init();	//xl init
	lsm6dsox_gyroscope_component.init();		//gy init

	//-------------------------------------------------------------------------------------
	// Setting up the xl and the gyro
	// Most embedded features use the accelerometer at high ODR
	// 417 Hz seemed to be the most widely used value in the constructor examples

	// Turn off components
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	// Set full scale
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_2G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);

	// Set Output Data Rate
	lsm6dsox_accelerometer_component.setDataRate(417.0f);
	lsm6dsox_gyroscope_component.setDataRate(417.0f);

	// Set component power mode
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);

	//-------------------------------------------------------------------------------------
	// Setup interrupts
	// The embedded features generate an interrupt (routed to INT1 in this example)
	// If the INT are not latched, you might have difficulties testing what has generated an interrupt
	lsm6dsox_int_notification_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_ALL_INT_LATCHED);

	//-------------------------------------------------------------------------------------
	// Setting up the features
	if(DETECT_WAKE_UP)
	{
		setup_wake_up();
		enableInterruptWakeUp();
		printf("Wake up detect enabled\n");
	}
	if(DETECT_SLEEP)
	{
		setup_sleep_state();
		enableInterruptSleepState();
		printf("Sleep detect enabled\n");
	}
	if(DETECT_FREE_FALL)
	{
		setup_free_fall();
		enableInterruptFreeFall();
		printf("Free fall detect enabled\n");
	}
	if(DETECT_6D_4D)
	{
		setup_orientation();
		enableInterruptOrientation();
		printf("Orientation detect enabled\n");
	}
	if(DETECT_TAP)
	{
		setup_single_double_tap();
		enableInterruptTap();
		printf("Tap and double tap detect enabled\n");
	}
	if(DETECT_TILT)
	{
		setup_tilt();
		enableInterruptTilt();
		printf("Tilt detect enabled\n");
	}
	
	//-------------------------------------------------------------------------------------
	// Main loop
	printf("\n\n----------------------------------\n");
	printf("Starting while loop\n");

	// Variables to print only when a value changes
	bool valueChanged	= false;
	uint8_t sleep_state = 0;
	uint8_t wake_up		= 0;
	uint8_t free_fall	= 0;
	uint8_t six_d		= 0;
	uint8_t double_tap	= 0;
	uint8_t single_tap	= 0;
	uint8_t tilt 		= 0;

	while (1) {

		// Get status for all interrupts on INT1
		lsm6dsox_all_sources_t all_source;
		lsm6dsox_all_sources_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &all_source);

		// Check if a value of the interrupt flags has changed 
		if(DETECT_WAKE_UP	 && all_source.wake_up != wake_up) 			valueChanged = true;
		if(DETECT_SLEEP		 && all_source.sleep_state != sleep_state) 	valueChanged = true;
		if(DETECT_FREE_FALL	 && all_source.free_fall != free_fall) 		valueChanged = true;
		if(DETECT_6D_4D		 && all_source.six_d != six_d) 				valueChanged = true;
		if(DETECT_TAP		 && all_source.single_tap != single_tap) 	valueChanged = true;
		if(DETECT_TAP		 && all_source.double_tap != double_tap) 	valueChanged = true;
		if(DETECT_TILT		 && all_source.tilt != tilt) 				valueChanged = true;

		if (valueChanged) {

			printf("\nStatus : \n");
			if(DETECT_WAKE_UP)		printf("wake_up  = %d \n", all_source.wake_up);
			if(DETECT_SLEEP)		printf("sleep_state  = %d \n", all_source.sleep_state);
			if(DETECT_FREE_FALL)	printf("free_fall  = %d \n", all_source.free_fall);
			if(DETECT_6D_4D)		printf("six_d  = %d \n", all_source.six_d);
			if(DETECT_TAP)			printf("single_tap  = %d \n", all_source.single_tap);
			if(DETECT_TAP)			printf("double_tap  = %d \n", all_source.double_tap);
			if(DETECT_TILT)			printf("tilt  = %d \n", all_source.tilt);

			// Set the new values for next comparison
			valueChanged = false;
			wake_up		 = all_source.wake_up;
			sleep_state	 = all_source.sleep_state;
			free_fall	 = all_source.free_fall;
			six_d		 = all_source.six_d;
			single_tap	 = all_source.single_tap;
			double_tap	 = all_source.double_tap;
			tilt		 = all_source.tilt;
		}
		
		ThisThread::sleep_for(50ms);
	}
	return 0;
}

//##################################################################################################
// Functions
//##################################################################################################
//------------------------------------------------------------------------------------------------------------------
// Wake up

/**
 * @brief Set up the wake up detection feature
 * 
 * 4 parameters :
 * 	- type of filter on the xl data : slope or high pass
 * 	- duration : number of ODR periods of detection before activating signal
 * 	- threshold weight : can be set to 1/256 or 1/64 of the xl full scale
 * 	- threshold : the real threshold is the threshold value multiplied by the threshold weight
 */
void setup_wake_up() {

	/* Apply slope filter on Wake-Up function */
	lsm6dsox_xl_hp_path_internal_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_USE_SLOPE);

	// Set number of ODR cycles in which the feature detects something
	lsm6dsox_wkup_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x00);

	//Select the weight of 1 LSb for threshold
	lsm6dsox_wkup_ths_weight_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_LSb_FS_DIV_256);

	// Set Wake-Up threshold: 1 LSb corresponds to FS_XL*threshold weight (1/256 or 1/64)
	lsm6dsox_wkup_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);
}

void enableInterruptWakeUp() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptWakeUp() {
	lsm6dsox_pin_int1_route_t int1_route;
	
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Sleep state

/**
 * @brief Set the sleep detection feature
 * 
 * * 6 parameters :
 * 	- type of filter on the xl data : slope or high pass
 * 	- wake up duration : number of ODR periods of detection before activity signal
 * 	- threshold weight : can be set to 1/256 or 1/64 of the xl full scale
 * 	- threshold : the real threshold is the threshold value multiplied by the threshold weight
 * 	
 *  - sleep duration : number of 512/ODR periods of detection before inactivity signal
 *  - mode_set : mode given to the XL and GY when inactivity is detected to save energy
 * 
 */
void setup_sleep_state() {
	
	// Apply high-pass digital filter on Wake-Up function
	lsm6dsox_xl_hp_path_internal_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_USE_SLOPE);

	// Set threshold weight
	lsm6dsox_wkup_ths_weight_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_LSb_FS_DIV_256);

	// Set duration for Activity detection to 2 * XL_full_scale / weight
	lsm6dsox_wkup_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	// Set duration for Inactivity detection 2 * 512 / ODR_XL
	lsm6dsox_act_sleep_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	// Set Activity/Inactivity threshold
	lsm6dsox_wkup_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);

	// Inactivity configuration : no impact, we are in Stationary/Motion detection mode
	lsm6dsox_act_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_XL_AND_GY_NOT_AFFECTED);
}

void enableInterruptSleepState() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.sleep_change = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}
void disableInterruptSleepState() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.sleep_change = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Free fall

/**
 * @brief Set the free fall detection feature
 * 
 * 2 parameters :
 * 	- duration : computed with N/XL_ODR N is written on 6 bits
 * 	- threshold : 8 different values, the bigger, the less strict it becomes, but it might come with false positives
 */
void setup_free_fall() {
	// Set Free Fall duration
	lsm6dsox_ff_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x01);

	// Set the free fall zone threshold (values inferior are considered as free fall)
	lsm6dsox_ff_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_FF_TSH_312mg);
}

void enableInterruptFreeFall() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptFreeFall() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Orientation
/**
 * @brief Set the orientation detection
 * 
 * This implementation is not working reliably despite all our tests and settings
 * Orienation are hardly detected and sometimes not following the right axis
 * 
 * 3 parameters:
 * 	- threshold : minimum angle at which the main axis has to be (kind of a cone)
 * 	- lpf2 : make accelerometer data go through the low pass filter 2 before being processed
 * 	- 6D_4D : switch between 4D (only X and Y) and 6D (Z too) detection
 */
void setup_orientation() {
	// Set threshold
	lsm6dsox_6d_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_DEG_50);

	// LPF2 on 6D/4D function selection.
	lsm6dsox_xl_lp2_on_6d_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_DISABLE);
	
	// Enable 4D mode
	lsm6dsox_4d_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_DISABLE);
}

void enableInterruptOrientation() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.six_d = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptOrientation() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.six_d = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Tap
/**
 * @brief Set up the single tap and double tap detection
 * 
 * 	This implementation is not working reliably despite all our tests and setting
 * 	The taps are not detected with enough fiability depsite really lowsettings, further testing in real conditions might be required		
 * 
 * settings :
 * 	- enable on X
 *  - enable on Y
 * 	- enable on Z
 * 
 * 	- X threshold
 * 	- Y threshold
 * 	- Z threshold
 * 
 * 	- shock : maximum period of time between the start of the tap and the moment the xl reading has to go back under th threshold
 * 	- duration : period of time in which the two taps have to be for it to be a double tap
 * 	- quiet : minimum period of time between the 2 taps of the double tap
 * 
 * 	- priority : if multiple axis are triggered, set the one with the priority
 * 
 * 	- single or double : select if single tap or double tap
 */

void setup_single_double_tap() {
	// Enable Tap detection on X, Y, Z
	lsm6dsox_tap_detection_on_z_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_y_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
	lsm6dsox_tap_detection_on_x_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);

	// Set Tap threshold : 1 LSb = (FS_XL)/(32)
	lsm6dsox_tap_threshold_x_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);
	lsm6dsox_tap_threshold_y_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);
	lsm6dsox_tap_threshold_z_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x08);

	/* Configure Single and Double Tap parameter
	 *
	 * The SHOCK field of the INT_DUR2 register sets the shock time
	 * 1LSb = 8 * (1/ODR_XL), if 0 then SHOCK = 4 * (1/ODR_XL)
	 * 
	 * For the maximum time between two consecutive detected taps
	 * 1 LSb = 32 * (1/ODR_XL), if 0 then SHOCK = 16 * (1/ODR_XL)
	 *
	 * The QUIET field of the INT_DUR2 register sets the min time between 2 shocks
	 * 1 LSb = 4 * (1/ODR_XL), if 0 then SHOCK = 2 * (1/ODR_XL)
	 */
	lsm6dsox_tap_shock_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x03);
	lsm6dsox_tap_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x07);
	lsm6dsox_tap_quiet_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x03);

	// If multiple axes are being triggered, choose the one with the priority to create the signal
	lsm6dsox_tap_axis_priority_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_XYZ);


	// Enable Single and Double Tap detection.
	lsm6dsox_tap_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_BOTH_SINGLE_DOUBLE);
}

void enableInterruptTap() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.double_tap = PROPERTY_ENABLE;
	int1_route.single_tap = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptTap() {
	lsm6dsox_pin_int1_route_t int1_route;

	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.double_tap = PROPERTY_DISABLE;
	int1_route.single_tap = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Tilt

/**
 * @brief Set up the tilt detection feature
 * 
 * Tilt detects when the device has moved more than 35 degrees in any duration
 * If so, the 0 point is set where the tilt was trigerrer, so 35 more degrees have to be generated
 * 
 * 1 parameter :
 * 	- enable tilt : enable tilt calculation
 */
void setup_tilt() {
	lsm6dsox_tilt_sens_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
}

void enableInterruptTilt() {
	lsm6dsox_pin_int1_route_t int1_route;
	
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptTilt() {
	lsm6dsox_pin_int1_route_t int1_route;
	
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

