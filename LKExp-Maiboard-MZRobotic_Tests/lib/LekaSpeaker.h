/**
******************************************************************************
* @file    LekaSpeaker.h
* @author  Yann Locatelli
* @brief   Speaker operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKASPEAKER_H__
#define __LEKASPEAKER_H__

#include "mbed.h"

class LekaSpeaker {
  public:
	LekaSpeaker(AnalogOut &interface);
	void init();

	void play440(int duration_sec);
	void playFrequency(uint16_t freq, int duration_sec);

	void runTest(int duration_sec = 30);

  private:
	AnalogOut &_interface;
	Ticker _ticker;

	const int _sample_size = 256;
	float _analog_out_data[256];
};

#endif	 // __LEKASPEAKER_H__
