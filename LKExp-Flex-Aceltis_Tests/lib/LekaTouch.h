/**
******************************************************************************
* @file    LekaTouch.h
* @author  Yann Locatelli
* @brief   Touch with proximity sensors operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKATOUCH_H__
#define __LEKATOUCH_H__

#include "mbed.h"
#include "MCP23017.h"

class LekaTouch
{
  public:
	LekaTouch(I2C &interface, DigitalOut &reset, DigitalIn &interrupt_A, DigitalIn &interrupt_B,
			  uint8_t write_address = 0xC0, uint8_t read_address = 0x4E);
	void init();
	void calibrate(uint8_t address);
	void readDAC(uint8_t address);
	void runTest(int n_repetition = 1);

  private:
	I2C &_interface;
	DigitalOut &_reset;
	DigitalIn &_interrupt_A;
	DigitalIn &_interrupt_B;
	uint8_t _write_address; //DAC - 4728
	uint8_t _read_address; //MUX - 23017
};

#endif	 // __LEKATOUCH_H__
