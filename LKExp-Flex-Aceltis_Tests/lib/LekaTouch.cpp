#include "LekaTouch.h"

#include "LekaInterfaces.h"

MCP23017 _multiplexer(PIN_I2C3_SDA, PIN_I2C3_SCL, 0x4E);

LekaTouch::LekaTouch(I2C &interface, DigitalOut &reset, DigitalIn &interrupt_A, DigitalIn &interrupt_B,
					 uint8_t write_address, uint8_t read_address)
	: _interface(interface),
	  _reset(reset),
	  _interrupt_A(interrupt_A),
	  _interrupt_B(interrupt_B),
	  _write_address(write_address),
	  _read_address(read_address)
{
	_reset = 1;
}

void check_config_MUX()
{
	for (uint16_t address = 0x00; address < 0x16; address += 0x01) {
		printf("address %x - value %x\n", address, _multiplexer.readRegister(address));
	}
	printf("\n");
}

void initMux()
{
	_multiplexer.reset();
	ThisThread::sleep_for(1s);
	char buffer[] = {0x00, 0x00};

	// SET IODIRA to 0x00 -> output to supply touch sensor
	// SET IODIRB to 0xFF -> input data from touch sensor (need pull-up)
	_multiplexer.inputOutputMask(0xFF00);	// IODIR = 01 & 00
	ThisThread::sleep_for(1s);

	// SET pull-up(1) to GPIO on B port
	_multiplexer.internalPullupMask(0xFF00);   // GPPU = 0D & 0C
	ThisThread::sleep_for(1s);

	_multiplexer.digitalWordWrite(0x003F);	 // GPIO = 13 & 12
	ThisThread::sleep_for(1s);

	return;
}

void LekaTouch::readDAC(uint8_t address)
{
	const int size = 24;
	char buffer[size];
	i2c3.read(address, buffer, size);

	printf("Configuration : \n %x", buffer[0]);
	for (int i = 1; i < size; i++) {
		if (i % 3 == 0) { printf("\n"); }
		printf(" %x", buffer[i]);
	}
	printf("\n");
}

void initDAC(uint8_t address)
{
	// BYTE 1 : VREF PD1 PD0 Gx D11 D10 D9 D8
	// BYTE 2 : D7 D6 D5 D4 D3 D2 D1 D0

	char vref[] = {0x80};
	i2c3.write(address, vref, 1);
	ThisThread::sleep_for(1s);
	char pd[] = {0xA0, 0x00};
	i2c3.write(address, pd, 2);
	ThisThread::sleep_for(1s);
	char gain[] = {0xC0};
	i2c3.write(address, gain, 1);
	ThisThread::sleep_for(1s);

	return;
}

void LekaTouch::calibrate(uint8_t address)
{
	uint16_t val  = 0x09C4;
	uint8_t val_a = 0x00;
	uint8_t val_b = 0x00;

	val_a = val >> 4;
	val_b = val;

	char data[]	 = {0x00, 0x00, 0x00};
	int feedback = 0;

	for (uint16_t val = 0x0000; val < 0x1000; val += 0x0100) {
		data[1] = (uint8_t)(val >> 8);
		data[2] = (uint8_t)val;
		for (uint8_t ch = 0x00; ch < 0x04; ch += 0x01) {
			data[0] = 0x40 + ch * 2;
			i2c3.write(address, data, 3);
			ThisThread::sleep_for(1s);
			i2c3.write(address + 0x02, data, 3);
			ThisThread::sleep_for(1s);
		}
		for (int i = 0; i < 5; i++) {
			feedback = _multiplexer.digitalWordRead();
			printf("Calibration : %x -> Detection : %x\n", val, feedback);
			ThisThread::sleep_for(1s);
		}
	}

	ThisThread::sleep_for(1s);
	readDAC(address);

	// // uint8_t val_h = 0x09;	// max 0x0F
	// // uint8_t val_l = 0xC4;
	// char ch[]	  = {0x58, (uint8_t)value >> 4, (uint8_t)value};
	// for (uint8_t i = 0x00; i < 0x07; i += 0x02) {
	// 	ch[0] = 0x58 + i;
	// 	i2c3.write(address, ch, 3);
	// 	ThisThread::sleep_for(1s);
	// }

	return;
}

void LekaTouch::init()
{
	initMux();
	initDAC(0xC0);
	initDAC(0xC2);

	return;
}

void LekaTouch::runTest(int n_repetition)
{
	printf("\nTest of Touch!\n");
	init();
	// check_config_MUX();

	char data[]	 = {0x00, 0x05, 0x00};
	for (uint8_t ch = 0x00; ch < 0x04; ch += 0x01) {
		data[0] = 0x40 + ch * 2;
		i2c3.write(0xC0, data, 3);
		ThisThread::sleep_for(1s);
		i2c3.write(0xC0 + 0x02, data, 3);
		ThisThread::sleep_for(1s);
	}
	// calibrate(0xC0);

	for (int i = 0; i < n_repetition; i++) {
		printf("%x\n", (uint8_t)(_multiplexer.digitalWordRead()>>8));
		ThisThread::sleep_for(1s);
	}

	printf("\n");
}
