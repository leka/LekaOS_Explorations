/**
******************************************************************************
* @file    main.cpp
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Mbed integration of a LSM6DSOX MLC configuration
******************************************************************************
*/

#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "LSM6DSOX_EmbeddedFeatures.h"
#include "mbed.h"

//#include "lsm6dsox_mlc.h"

using namespace EmbeddedFeatures;

//##################################################################################################
// Defines
//##################################################################################################

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT1 (A5)

//##################################################################################################
// Prototypes
//##################################################################################################

void ISR_INT1();

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

// Flags for interrupts
bool int1Flag = false;

//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	// First message to serial
	printf("\n----------------------------------\n");
	printf("Starting a new run!\n\n");

	//-------------------------------------------------------------------------------------
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

	// Restore default configuration
	printf("Restore default config\n");
	lsm6dsox_embeddedFeatures.restoreDefaultConfiguration();

	//-------------------------------------------------------------------------------------
	// Disabling I3C
	printf("Disable I3C\n");
	lsm6dsox_embeddedFeatures.disableI3C();	  // disabling I3C on sensor
	INT_1_LSM6DSOX.input();	  // setting up INT1 pin as input (for interrupts to work)

	//-------------------------------------------------------------------------------------
	// Sensors init
	printf("Components init\n");
	lsm6dsox_embeddedFeatures.init();		//embedded features init
	lsm6dsox_accelerometer_component.init();	//xl init
	lsm6dsox_gyroscope_component.init();		//gy init

	//-------------------------------------------------------------------------------------
	// Setting up the xl and the gyro
	printf("Setting up xl and gyro\n");

	// Turn off Sensors
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	// Set full scale
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_2G);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_2000DPS);

	// Set Output Data Rate
	lsm6dsox_accelerometer_component.setDataRate(417.0f);
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setDataRate(417.0f);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);

	//-------------------------------------------------------------------------------------
	// Setup interrupts
	/* Enable LIR */
	lsm6dsox_int_notification_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_ALL_INT_LATCHED);

	//-------------------------------------------------------------------------------------
	bool valueChanged	= false;
	uint8_t sleep_state = 255;
	uint8_t wake_up		= 255;
	uint8_t free_fall	= 255;
	uint8_t six_d		= 255;
	uint8_t double_tap	= 255;
	uint8_t single_tap	= 255;


	// setup_free_fall();
	// enableInterruptFreeFall();
	// setup_wake_up();
	// enableInterruptWakeUp();
	// setup_sleep_state();
	// enableInterruptSleepState();
	setup_orientation();
	enableInterruptOrientation();





	//-------------------------------------------------------------------------------------
	// Verify settings
	printf("\n\n----------------------------------\n");
	printf("Actual sensor settings\n");

	printf("\nInterrupts set:\n");
	lsm6dsox_pin_int1_route_t route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &route);

	if(route.sleep_change) printf("sleep_state\n");
	if(route.wake_up) printf("wake_up\n");
	if(route.free_fall) printf("free_fall\n");
	if(route.six_d) printf("six_d\n");
	if(route.double_tap) printf("double_tap\n");
	if(route.single_tap) printf("single_tap\n");

	lsm6dsox_lir_t lir;
	lsm6dsox_int_notification_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &lir);
	printf("\nState of int notif: %d\n(3 = latched)\n", lir);

	lsm6dsox_xl_hm_mode_t pm;
	lsm6dsox_xl_power_mode_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &pm);
	printf("\nPower mode : %d\n", pm);
	// LSM6DSOX_HIGH_PERFORMANCE_MD  = 0
	// LSM6DSOX_LOW_NORMAL_POWER_MD  = 1
	// LSM6DSOX_ULTRA_LOW_POWER_MD   = 2
	
	//-------------------------------------------------------------------------------------
	// Main loop
	printf("\n\n----------------------------------\n");
	printf("Starting while loop\n");
	while (1) {
		lsm6dsox_all_sources_t all_source;
		// Get status for all interrupts on INT1
		lsm6dsox_all_sources_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &all_source);

		// if(all_source.sleep_state != sleep_state) valueChanged = true;
		// if(all_source.wake_up != wake_up) valueChanged = true;
		// if(all_source.free_fall != free_fall) valueChanged = true;
		if(all_source.six_d != six_d) valueChanged = true;
		// if(all_source.double_tap != double_tap) valueChanged = true;
		// if(all_source.single_tap != single_tap) valueChanged = true;

		if (valueChanged) {
			printf("\nAll status : \n");
			//printf("sleep_state  = %d \n", all_source.sleep_state);
			// printf("wake_up  = %d \n", all_source.wake_up);
			// printf("free_fall  = %d \n", all_source.free_fall);
			printf("six_d  = %d \n", all_source.six_d);
			// printf("double_tap  = %d \n", all_source.double_tap);
			// printf("single_tap  = %d \n", all_source.single_tap);

			valueChanged = false;
			sleep_state	 = all_source.sleep_state;
			wake_up		 = all_source.wake_up;
			free_fall	 = all_source.free_fall;
			six_d		 = all_source.six_d;
			double_tap	 = all_source.double_tap;
			single_tap	 = all_source.single_tap;
		}

		// // SLEEP
		// if (all_source.sleep_state)
		// {
		// 	printf("Inactivity Detected\n");
		// }

		// // WAKE UP
		// if (all_source.wake_up)
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
		// if (all_source.free_fall && DETECT_FREE_FALL)
		// {
		// 	printf("Free Fall Detected\n");
		// }

		//ORIENTATION
		if (all_source.six_d)
		{
			printf("6D Orientation switched to ");
			if (all_source.six_d_xh)
				printf("XH\n");
			if (all_source.six_d_xl)
				printf("XL\n");
			if (all_source.six_d_yh)
				printf("YH\n");
			if (all_source.six_d_yl)
				printf("YL\n");
			if (all_source.six_d_zh)
				printf("ZH\n");
			if (all_source.six_d_zl)
				printf("ZL\n");
		}

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

		// ThisThread::sleep_for(50ms);
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
void ISR_INT1() { int1Flag = true; }

// //------------------------------------------------------------------------------------------------------------------

/**
 * @brief Set the activity detection feature
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
	/* Set Free Fall duration to 3 and 6 samples event duration */
	lsm6dsox_ff_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x01);
	lsm6dsox_ff_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_FF_TSH_312mg);
}

/**
 * @brief Enable free fall routing on INT1
 * 
 */
void enableInterruptFreeFall() {
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

/**
 * @brief Disable free fall routing on INT1
 * 
 */
void disableInterruptFreeFall() {
	lsm6dsox_pin_int1_route_t int1_route;
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.free_fall = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

//------------------------------------------------------------------------------------------------------------------
// Orientation
/**
 * @brief Set the up orientation object
 * 
 */
void setup_orientation() {
	/* Set threshold to 60 degrees. */
	lsm6dsox_6d_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_DEG_60);

	/* LPF2 on 6D/4D function selection. */
	lsm6dsox_xl_lp2_on_6d_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_DISABLE);
	/*
	 * To enable 4D mode uncomment next line.
	 * 4D orientation detection disable Z-axis events.
	 */
	// lsm6dsox_4d_mode_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
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
void setup_single_double_tap() {
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
void setup_tilt() {
	/* Enable Tilt in embedded function. */
	lsm6dsox_tilt_sens_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), PROPERTY_ENABLE);
}

void enableInterruptTilt() {
	lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment if interrupt generation on Tilt INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);

	/* Uncomment to have interrupt latched */
	lsm6dsox_int_notification_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(),
								  LSM6DSOX_ALL_INT_LATCHED);
}

void disableInterruptTilt() {
	lsm6dsox_pin_int1_route_t int1_route;
	/* Uncomment if interrupt generation on Tilt INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.tilt = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

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

	/* Apply high-pass digital filter on Wake-Up function */
	lsm6dsox_xl_hp_path_internal_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_USE_SLOPE);

	// lsm6dsox_wkup_dur_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &val);
	lsm6dsox_wkup_dur_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x00);

	lsm6dsox_wkup_ths_weight_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), LSM6DSOX_LSb_FS_DIV_256);

	/* Set Wake-Up threshold: 1 LSb corresponds to FS_XL/2^6 */
	lsm6dsox_wkup_threshold_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), 0x02);
}

void enableInterruptWakeUp() {
	lsm6dsox_pin_int1_route_t int1_route;
	/* Enable if interrupt generation on Wake-Up INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_ENABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}

void disableInterruptWakeUp() {
	lsm6dsox_pin_int1_route_t int1_route;
	/* Enable if interrupt generation on Wake-Up INT2 pin */
	lsm6dsox_pin_int1_route_get(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), &int1_route);
	int1_route.wake_up = PROPERTY_DISABLE;
	lsm6dsox_pin_int1_route_set(lsm6dsox_embeddedFeatures.TMP_getIoFunc(), int1_route);
}