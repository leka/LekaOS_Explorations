/**
******************************************************************************
* @file    LekaBrightness.h
* @author  Yann Locatelli
* @brief   Light sensor operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKABRIGHTNESS_H__
#define __LEKABRIGHTNESS_H__

#include "mbed.h"

class LekaBrightness {
  public:
	LekaBrightness(AnalogIn&);
	float readValuePercentage();
	uint16_t readValueNormalized();

	void runTest(int n_repetition = 1);

  private:
	AnalogIn &_interface;
};

#endif	 // __LEKABRIGHTNESS_H__
