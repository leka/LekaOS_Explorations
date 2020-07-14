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
#include "ESP8266Interface.h"
#include "mbed.h"
/*
QSPI qspi1(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_1);	  //+ Mode polarity at 0
QSPI qspi2(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_2);	  //+ Mode polarity at 0
QSPI qspi3(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_3);	  //+ Mode polarity at 0
*/
extern SDBlockDevice sd;

extern I2C i2c1;
extern DigitalIn int_imu;
extern DigitalIn int_mag;
extern DigitalIn int_temp;

extern AnalogIn microphone;

extern AnalogIn brightness;
/*
PwmOut motor_left(PIN_ENA);
PwmOut motor_right(PIN_ENB);
*/
extern SPI spi5;
extern DigitalOut ble_reset;
//DigitalIn ble_interrupt(PIN_IRQ_BLE);

extern ESP8266Interface wifi;
// extern BufferedSerial uart2;
// extern DigitalOut wifi_reset;
// extern DigitalOut wifi_enable;
/*
Serial uart6(PIN_USART6_BTTX, PIN_USART6_BTRX);
DigitalOut bluetooth_reset(PIN_RST_BM64);
DigitalOut bluetooth_wakeup(PIN_BT_WAKEUP);
*/
extern DigitalOut sound_on;
extern AnalogOut sound_out;

#endif	 // __LEKAINTERFACES_H__