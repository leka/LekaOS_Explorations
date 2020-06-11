/**
******************************************************************************
* @file    LekaMotors.h
* @author  Yann Locatelli
* @brief   Motors operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAMOTORS_H__
#define __LEKAMOTORS_H__

#include "mbed.h"

class LekaMotors {
  public:
	LekaMotors(PwmOut &l_mot, PwmOut &r_mot, PinName in1, PinName in2, PinName in3, PinName in4);

	void init(float period = 2000);
	void setSpeed(float ratio, uint8_t direction);
	void runTest(int duration_sec = 30);

  private:
	DigitalOut _in1;
	DigitalOut _in2;
	DigitalOut _in3;
	DigitalOut _in4;

	PwmOut &_l_mot;
	PwmOut &_r_mot;

	Ticker _ticker;
};

// Motor : 25kHz Commutation frequency
// https://electronics.stackexchange.com/questions/309056/l298n-pwm-frequency

#endif	 // __LEKAMOTORS_H__
