/**
******************************************************************************
* @file    LekaFirmware.h
* @author  Yann Locatelli
* @brief   Firmware operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAFIRMWARE_H__
#define __LEKAFIRMWARE_H__

#include "mbed.h"
#include "LekaInterfaces.h"

class LekaFirmware {
  public:
	LekaFirmware(DigitalOut &ce1_disable, DigitalOut &ce2_disable, DigitalOut &ce3_disable);
	void init();
	void runTest();

  private:
    DigitalOut _ce1_disable;
    DigitalOut _ce2_disable;
    DigitalOut _ce3_disable;
};

#endif	 // __LEKAFIRMWARE_H__
