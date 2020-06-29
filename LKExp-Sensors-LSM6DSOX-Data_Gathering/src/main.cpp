
#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_ComponentAccelerometer.h"
#include "LSM6DSOX_ComponentGyroscope.h"
#include "LSM6DSOX_DataGatherer.h"
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

//##################################################################################################
// Prototypes
//##################################################################################################

void tickerCB();

//##################################################################################################
// Global Variables
//##################################################################################################
I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

// Component::LSM6DSOX_DataGatherer dataGatherer(lsm6dsox_i2c, PIN_LSM6DSOX_INT1, 10.0f, USBTX, USBRX, MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE, tickerCB);
Component::LSM6DSOX_DataGatherer dataGatherer(lsm6dsox_i2c, PIN_LSM6DSOX_INT1, 10.0f, USBTX, USBRX, 230400, tickerCB);

DigitalInOut INT_1_LSM6DSOX(PIN_LSM6DSOX_INT1, PIN_OUTPUT, PullNone, 0); // This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

//UnbufferedSerial serial(USBTX, USBRX);
//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	// Time for sensor init
	ThisThread::sleep_for(10ms);	//waiting for sensor startup


	//Greetings
	printf("  Starting a new run\n\n");

	// Restore default configuration
	dataGatherer.restoreDefaultConfiguration();

	// Disable I3C and setting INT1 pin as input
	dataGatherer.disableI3C();
	INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)

	//Components init
	dataGatherer.init();

	// Enable Block Data Update
	lsm6dsox_block_data_update_set(dataGatherer.TMP_getIoFunc(), PROPERTY_ENABLE);

	dataGatherer.setDataRate(104);

	dataGatherer.printHeader();


	printf("Starting while loop\n\n");

	dataGatherer.startTicker();

	while (1) 
	{
	}

	return 0;
}

void tickerCB(){
	dataGatherer.onTick();
}



//##################################################################################################
// Event queue test
//##################################################################################################




// // // #include "mbed.h"
// // // #include "mbed_events.h"


// // // class Test{


// // // 	public:
// // // 	Test() : led1(LED1)
// // // 	{
// // // 		// Request the shared queue
// // // 		// EventQueue *queue = mbed_event_queue();
		
// // // 		//tick.attach(queue->event(&onTick), 500ms);

// // // 	}

// // // 	void onTick(void)
// // // 	{
// // // 		//pc.write("Tick\n", sizeof("Tick\n")/sizeof(char));
// // // 		printf("Tick\n");
// // // 		led1 = !led1;
// // // 	}

// // // 	DigitalOut led1;
// // // 	Ticker _tick;
// // // };





// // // InterruptIn sw(BUTTON1);
// // // UnbufferedSerial pc(USBTX, USBRX);

// // // Test test;

// // // void callBack()
// // // {
// // // 	test.onTick();
// // // }

// // // int main()
// // // {
// // // 	EventQueue *queue = mbed_event_queue();
		
// // // 	test._tick.attach(queue->event(&callBack), 500ms);


// // // 	while(1){

// // // 	}
// // // }

