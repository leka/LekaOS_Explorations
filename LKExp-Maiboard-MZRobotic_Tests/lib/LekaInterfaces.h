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
#include "SDBlockDevice.h"
#include "QSPIFBlockDevice.h"
#include "ESP8266Interface.h"
#include "mbed.h"

extern QSPIFBlockDevice qspi_bd;	  //+ Mode polarity at 0
extern DigitalOut ce1_unselect;
extern DigitalOut ce2_unselect;
extern DigitalOut ce3_unselect;
/*
QSPI qspi2(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_2);	  //+ Mode polarity at 0
QSPI qspi3(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_3);	  //+ Mode polarity at 0
*/
extern SDBlockDevice sd;
extern DigitalInOut sd_on;

extern I2C i2c1;
extern DigitalIn int_imu;
extern DigitalIn int_mag;
extern DigitalIn int_temp;

extern AnalogIn microphone;

extern AnalogIn brightness;

extern PwmOut motor_left;
extern PwmOut motor_right;

extern SPI spi5;
extern DigitalOut ble_reset;
//DigitalIn ble_interrupt(PIN_IRQ_BLE);

extern ESP8266Interface wifi;
// extern BufferedSerial uart2;
// extern DigitalOut wifi_reset;
// extern DigitalOut wifi_enable;

// extern BufferedSerial uart6;
extern DigitalOut bluetooth_reset;
extern DigitalOut bluetooth_wakeup;

extern DigitalOut sound_on;
extern AnalogOut sound_out;

#endif	 // __LEKAINTERFACES_H__