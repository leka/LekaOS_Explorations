/**
******************************************************************************
* @file    LekaBluetooth.h
* @author  Yann Locatelli
* @brief   Bluetooth operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKABLUETOOTH_H__
#define __LEKABLUETOOTH_H__

#include "mbed.h"
//#include "drv_bm64.h"

class LekaBluetooth {
  public:
	LekaBluetooth(PinName pin_tx, PinName pin_rx, DigitalOut &reset, DigitalOut &wake_up);

	void runTest();

  private:
	BufferedSerial _bm64;
	DigitalOut _reset;
	DigitalOut _wake_up;
};

#endif	 // __LEKABLUETOOTH_H__
