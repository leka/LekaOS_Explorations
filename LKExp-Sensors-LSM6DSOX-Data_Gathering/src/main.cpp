
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


//##################################################################################################
// Prototypes
//##################################################################################################
void printHeader(Lsm6dsoxDataToSend &dataToSend, std::array<string, 6> &headers);
void printData(Component::AccelerometerData &ac, Component::GyroscopeData &gy, Lsm6dsoxDataToSend &dataToSend);

static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

void onSerialReceived();
void parseCommand();

bool isFloat(string s);

//##################################################################################################
// Global Variables
//##################################################################################################
I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);

Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1); 
Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT1);

DigitalInOut INT_1_LSM6DSOX(PIN_LSM6DSOX_INT1, PIN_OUTPUT, PullNone, 0); // This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

UnbufferedSerial pc(USBTX, USBRX);
DigitalOut led1(LED1);

string commandString = "";
bool commandStringComplete = false;


bool serialReceivedFlag = false;


// Variables for commands
bool dataGatheringRunning = false;
float dataRate;
int periodMS;
Lsm6dsoxDataToSend dataToSend = {1, 1, 1, 1, 1, 1};


//##################################################################################################
// Main
//##################################################################################################

int main(void) {
	// Time for sensor init
	ThisThread::sleep_for(10ms);	//waiting for sensor startup

	//Greetings
	printf("  Starting a new run\n\n");

	/* Initialize mems driver interface */
	stmdev_ctx_t ctx;
	ctx.write_reg = platform_write;
	ctx.read_reg  = platform_read;
	ctx.handle    = NULL;

	// Restore default configuration
	uint8_t rst;
	lsm6dsox_reset_set(&ctx, PROPERTY_ENABLE);
	do {
		lsm6dsox_reset_get(&ctx, &rst);
	} while (rst);

	// Disable I3C and setting INT1 pin as input
	lsm6dsox_i3c_disable_set(&ctx, LSM6DSOX_I3C_DISABLE);		//disabling I3C on sensor
	// INT_1_LSM6DSOX.input();			//setting up INT1 pin as input (for interrupts to work)

    //Attach IRQ on serial receive
	commandString.reserve(50);
    pc.attach(&onSerialReceived, UnbufferedSerial::RxIrq);

	//Components init
	lsm6dsox_accelerometer_component.init();
	lsm6dsox_gyroscope_component.init();

	// Turn off Sensors
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::OFF);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::OFF);

	// Enable Block Data Update
	lsm6dsox_block_data_update_set(&ctx, PROPERTY_ENABLE);

	//Components config
	lsm6dsox_accelerometer_component.setPowerMode(Component::PowerMode::NORMAL);
	lsm6dsox_gyroscope_component.setPowerMode(Component::PowerMode::NORMAL);

	// config data rate and period of polling
	dataRate = 104.0f;
	periodMS = (1/dataRate)*1000;

	lsm6dsox_gyroscope_component.setDataRate(dataRate);
	lsm6dsox_accelerometer_component.setDataRate(dataRate);


	//Datalog config
	std::array<string, 6> headers = {"A_X [mg]", "A_Y [mg]", "A_Z [mg]", "G_X [mg]", "G_Y [mg]", "G_Z [mg]" };
	

	Component::GyroscopeData gy;
	Component::AccelerometerData ac;;

	printHeader(dataToSend, headers);
	printf("Starting while loop\n\n");
	while (1) {
		if(commandStringComplete) parseCommand();

		if(dataGatheringRunning)
		{
			lsm6dsox_accelerometer_component.getData(ac.data);
			lsm6dsox_gyroscope_component.getData(gy.data);

			printData(ac, gy, dataToSend);
		}
		ThisThread::sleep_for(periodMS);
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


void onSerialReceived(){
	//printf("Reading serial\n");
	char chr = '\0';
    while (pc.readable()) {
        led1 = !led1;
        pc.read(&chr, 1);
		if(chr != '\n' && chr != '\0' && chr != '\r') commandString.push_back(chr);
		else if(chr != '\r') commandStringComplete = true;
	}
}


void parseCommand()
{
	printf("Your command is: '%s'\n", commandString.c_str());
	// printf("Size of the command: %d\n", commandString.length());
	if(commandString == "start"){
		dataGatheringRunning = true;
	}
	else if(commandString == "stop") {
		dataGatheringRunning = false;
	}
	else if(commandString.substr(0, 5) == "rate "){
		commandString = commandString.substr(5, commandString.length()-5);
		printf("Your number is: '%s'\n", commandString.c_str());
		float rate;
		if(isFloat(commandString)){
			rate = stof(commandString);
			if(rate > 0.0 && rate <= 104.0){
				dataRate = rate;
				periodMS = (1/dataRate)*1000;
				printf("New data rate: %d\n\n", (int)rate);
			}
			else printf("Data rate out of bounds, data rate still set to : '%d'\n", (int)dataRate);
		}
		else printf("Data rate not valid, data rate still set to : '%d'\n", (int)dataRate);
	}
	else if(commandString.substr(0, 6) == "range "){
		printf("Changing range\n");
		commandString = commandString.substr(6, commandString.length()-6);
		if(commandString.substr(0, 3) == "xl "){
			printf("On accelerometer\n");
			
			commandString = commandString.substr(3, commandString.length()-3);
			Component::AccelerometerRange range;
			lsm6dsox_accelerometer_component.getRange(range);
			
			if(commandString == "2G") range = Component::AccelerometerRange::_2G;
			else if(commandString == "4G") range = Component::AccelerometerRange::_4G;
			else if(commandString == "8G") range = Component::AccelerometerRange::_8G;
			else if(commandString == "16G") range = Component::AccelerometerRange::_16G;
			else printf("Invalid range input, range is still %dG\n", range);
			
			lsm6dsox_accelerometer_component.setRange(range);
			printf("Accelerometer range set to: %dG\n", range);
		}
		else if(commandString.substr(0, 3) == "gy "){
			printf("On gyroscope\n");

			commandString = commandString.substr(3, commandString.length()-3);	
			Component::GyroscopeRange range;
			lsm6dsox_gyroscope_component.getRange(range);

			if(commandString == "125DPS") range = Component::GyroscopeRange::_125DPS;
			else if(commandString == "250DPS") range = Component::GyroscopeRange::_250DPS;
			else if(commandString == "500DPS") range = Component::GyroscopeRange::_500DPS;
			else if(commandString == "1000DPS") range = Component::GyroscopeRange::_1000DPS;
			else if(commandString == "2000DPS") range = Component::GyroscopeRange::_2000DPS;
			else printf("Invalid range input, range is still %dDPS\n", range);

			lsm6dsox_gyroscope_component.setRange(range);
			printf("Gyroscope range set to: %dDPS\n", range);
		}
		else printf("Not a valid device chosen (xl or gy)\n");
	}
	else if(commandString == "help"){
		printf("start \n");
		printf("stop \n");
		printf("rate [value] \n");
		printf("range [device] [value] \n");
	}
	else if(commandString.substr(0, 5) == "data "){
		printf("Selecting data to print\n");
		commandString = commandString.substr(5, commandString.length()-5);

		int cpt = 0;
		for(int i = 0; i < commandString.length(); ++i)
		{
			if(i%2 == 0)
			{
				printf("char: %c num %d\n", commandString[i], cpt);
				printf("Should be a number\n");
				if(commandString[i] == '0') {
					dataToSend.dataArray[cpt] = false;
					printf("is a 0\n");
				}
				else {
					dataToSend.dataArray[cpt] = true;
					printf("is a 1\n");
				}
				cpt++;
			}
			if(cpt == 6) break;
		}
		printf("The pattern of data that will be sent is: ");
		for(int i = 0; i<6; ++i)
		{
			printf("%d ",(int)dataToSend.dataArray[i]);
		}
		printf("\n");
	}
	else printf("Invalid command, type 'help' for more info\n");


	commandStringComplete = false;
	commandString = "";
}


bool isFloat(string s){
	int numPoints = 0;
	for(int i = 0; i<s.length(); ++i)
	{	
		if(s[i] == '.')
		{
			if((numPoints == 0) && (i != 0) && (i != s.length()-1)) numPoints++;
			else return false;
		}
		else if((s[i] < '0') || (s[i] > '9')) return false;
	}
	return true;
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