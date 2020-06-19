
#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "mbed.h"

//##################################################################################################
// Defines
//##################################################################################################

#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT1 (A5)

//##################################################################################################
// Structs
//##################################################################################################

union Lsm6dsoxDataToSend {
	std::array<bool, 6> dataArray;
	struct {
		bool acc_x;
		bool acc_y;
		bool acc_z;
		bool gyr_x;
		bool gyr_y;
		bool gyr_z;
	} data;
};

union Int1Route{
	std::array<uint8_t, 45> array;
	lsm6dsox_pin_int1_route_t route;
};

//##################################################################################################
// Prototypes
//##################################################################################################
void printHeader(Lsm6dsoxDataToSend &dataToSend, std::array<string, 6> &headers);
void printData(Component::AccelerometerData &ac, Component::GyroscopeData &gy, Lsm6dsoxDataToSend &dataToSend);

static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

void isrINT1();

//##################################################################################################
// Global Variables
//##################################################################################################
I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1); 
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);

DigitalInOut INT_1_LSM6DSOX(PIN_LSM6DSOX_INT1, PIN_OUTPUT, PullNone, 0); // This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

bool isrINT1Flag = false;
bool xlDataReadyFlag = false;
bool gyDataReadyFlag = false;

//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	// Time for sensor init
	ThisThread::sleep_for(10ms);	//waiting for sensor startup

	//Greetings
	printf("       Starting a new run\n\n");

	/* Initialize mems driver interface */
	stmdev_ctx_t ctx;
	ctx.write_reg = platform_write;
	ctx.read_reg  = platform_read;
	ctx.handle    = NULL;

	// Disable I3C and setting INT1 pin as input
	lsm6dsox_i3c_disable_set(&ctx, LSM6DSOX_I3C_DISABLE);		//disabling I3C on sensor
	INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)


	//Components init
	lsm6dsox_accelerometer_component.init();
	lsm6dsox_gyroscope_component.init();

	//Components config
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_accelerometer_component.setDataRate(12.5f);
	lsm6dsox_accelerometer_component.setRange(Component::AccelerometerRange::_2G);

	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setDataRate(12.5f);
	lsm6dsox_gyroscope_component.setRange(Component::GyroscopeRange::_500DPS);


	//interrupt config accelerometer
	Component::AccelerometerEventsOnInterrupt xlIntEv;
	xlIntEv.event_on_interrupt.data_ready = 1;
	lsm6dsox_accelerometer_component.setEventsOnInterrupt(xlIntEv.component_events_on_interrupt);

	//interrupt config gyro
	Component::GyroscopeEventsOnInterrupt gyIntEv;
	gyIntEv.event_on_interrupt.data_ready = 1;
	lsm6dsox_gyroscope_component.setEventsOnInterrupt(gyIntEv.component_events_on_interrupt);

	//enable INT1 IRQ
	lsm6dsox_accelerometer_component.enableInterrupt();
	lsm6dsox_accelerometer_component.attachInterrupt(&isrINT1);

	lsm6dsox_int_notification_set(&ctx, LSM6DSOX_ALL_INT_LATCHED); 

	//Datalog config
	std::array<string, 6> headers = {"A_X [mg]", "A_Y [mg]", "A_Z [mg]", "G_X [mg]", "G_Y [mg]", "G_Z [mg]" };
	Lsm6dsoxDataToSend dataToSend = {1, 1, 0, 1, 1, 0};
	
	float dataRate = 52.0f;
	int periodMS = (1/dataRate)*1000;


	Component::GyroscopeData gy;
	Component::AccelerometerData ac;

	// TODO
	// apparently the latched interrupts are not cleared by all_sources_get

	//emptying latched interrupts
	lsm6dsox_all_sources_t status;
	lsm6dsox_all_sources_get(&ctx, &status);

	printf("Int on xl: %d\n", status.drdy_xl);
	printf("Int on gy: %d\n", status.drdy_g);

	printf("after reading: \n");
	lsm6dsox_all_sources_get(&ctx, &status);

	printf("Int on xl: %d\n", status.drdy_xl);
	printf("Int on gy: %d\n", status.drdy_g);


	printHeader(dataToSend, headers);
	
	while (1) {

		if(isrINT1Flag)
		{
			printf("interrupt on INT1\n");

			Component::AccelerometerEvents xl_event;
			Component::GyroscopeEvents gy_event;
			lsm6dsox_accelerometer_component.getEventStatus(xl_event.component_events);
			lsm6dsox_gyroscope_component.getEventStatus(gy_event.component_events);
			if(xl_event.event.data_ready) xlDataReadyFlag = true;
			if(gy_event.event.data_ready) gyDataReadyFlag = true;

			isrINT1Flag = false;
		}
		
		if(xlDataReadyFlag && gyDataReadyFlag)
		{
			printf("both data rdy\n");

			lsm6dsox_accelerometer_component.getData(ac.data);
			lsm6dsox_gyroscope_component.getData(gy.data);

			printData(ac, gy, dataToSend);
			xlDataReadyFlag = false;
			gyDataReadyFlag = false;
		}


		lsm6dsox_all_sources_get(&ctx, &status);
		ThisThread::sleep_for(1s);
	}
	return 0;
}

//##################################################################################################
// Functions
//##################################################################################################

void printHeader(Lsm6dsoxDataToSend &dataToSend, std::array<string, 6> &headers)
{
	

	printf("\n\nSTEVAL-MKI197V1 (LSM6DSOX)\n\n");

	for( int i = 0; i<6; i++){
		if(dataToSend.dataArray[i]){
			printf(headers[i].c_str());
			printf("\t");
		}
	}
	printf("\n");	
}

void printData(Component::AccelerometerData &ac, Component::GyroscopeData &gy,Lsm6dsoxDataToSend &dataToSend)
{
	for(int i = 0; i < 3; ++i)
	{
		if(dataToSend.dataArray[i]) printf("%d\t", (int)ac.data[i]);
	}
	for(int i = 3; i < 6; ++i)
	{
		if(dataToSend.dataArray[i]) printf("%d\t", (int)gy.data[i-3]);
	}
	printf("\n");
}


static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    //HAL_I2C_Mem_Write(handle, LSM6DSOX_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
    lsm6dsox_i2c.write(reg, len, bufp);
  return 0;
}


static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    //HAL_I2C_Mem_Read(handle, LSM6DSOX_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
    lsm6dsox_i2c.read(reg, len, bufp);
  return 0;
}

void isrINT1()
{
	isrINT1Flag = true;
}



// //################################################################################################
// // Blink (in case of real problems)

// // //Connection test
// // int main(void) {

// // 	DigitalOut led(LED1);

// // 	while (true) {
// //         led = !led;
// // 		static int i = 0;
// // 		printf("%d\n", i);
// // 		++i;
// // 		ThisThread::sleep_for(1s);
// // 	}

// // 	return 0;
// // }
// //################################################################################################