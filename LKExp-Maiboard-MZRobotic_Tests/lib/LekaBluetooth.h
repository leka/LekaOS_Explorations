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
	LekaBluetooth(PinName pin_tx, PinName pin_rx);

	void runTest();
private:
BufferedSerial _bm64;
};

#endif	 // __LEKABLUETOOTH_H__
