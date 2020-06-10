/**
******************************************************************************
* @file    LekaMicrophone.h
* @author  Yann Locatelli
* @brief   Microphone operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAMICROPHONE_H__
#define __LEKAMICROPHONE_H__

#include "mbed.h"

class LekaMicrophone {
  public:
	LekaMicrophone(AnalogIn);
	float readValuePercentage();
	uint16_t readValueNormalized();

	void runTest(int n_repetition = 1);

  private:
	AnalogIn &_interface;
};

#endif	 // __LEKAMICROPHONE_H__
