/**
******************************************************************************
* @file    LekaInterfaces.h
* @author  Yann Locatelli
* @brief   Temporary file gathering mbed-interfaces based on LekaPins.h
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAINTERFACES_H__
#define __LEKAINTERFACES_H__

#include "LekaPins.h"
#include "mbed.h"

// extern BufferedSerial rfid;

extern I2C i2c3;

extern SPI leds_ears;
extern SPI leds_belt;

extern DigitalOut mux_reset;
extern DigitalIn mux_inta;
extern DigitalIn mux_intb;

#endif	 // __LEKAINTERFACES_H__
