/**
******************************************************************************
* @file    LekaLEDs.h
* @author
* @brief   LEDs reader operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKALEDS_H__
#define __LEKALEDS_H__

#include "mbed.h"

class LekaLEDs
{
  public:
	LekaLEDs(SPI &leds_ears, SPI &leds_belt);
	void init();
	void runTest(int n_repetition = 1);

  private:
	SPI &_leds_ears;
	SPI &_leds_belt;
	int n_leds_ears = 2;
	int n_leds_belt = 20;
};

#endif	 // __LEKALEDS_H__
