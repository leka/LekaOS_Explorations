/**
******************************************************************************
* @file    LekaRFID.h
* @author  Yann Locatelli
* @brief   RFID reader operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKARFID_H__
#define __LEKARFID_H__

#include "mbed.h"
#include "LekaPins.h"

class LekaRFID {
  public:
	LekaRFID(PinName uart_tx, PinName uart_rx);
	void init();
    void rfidWrite(uint8_t cmd[], uint8_t len);
    void rfidRead();
	// void on_rx_interrupt();
	void runTest(int duration_sec = 30);

  private:
	int _baud = 57600;	 // default
	BufferedSerial _interface;
    Ticker _ticker;
};

#endif	 // __LEKARFID_H__