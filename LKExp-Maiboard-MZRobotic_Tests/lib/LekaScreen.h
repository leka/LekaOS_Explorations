/**
******************************************************************************
* @file    LekaScreen.h
* @author  Yann Locatelli
* @brief   Screen operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKASCREEN_H__
#define __LEKASCREEN_H__

#include "mbed.h"
#include "stm32f769i_discovery_lcd.h"

class LekaScreen {
  public:
	LekaScreen();

	void init();
    void fadingAnimation(int duration_sec);
	void runTest(int duration_sec = 30);

  private:
	Ticker _ticker;
};

#endif	 // __LEKASCREEN_H__
