/**
******************************************************************************
* @file    LekaSD.h
* @author  Yann Locatelli
* @brief   SD operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKASD_H__
#define __LEKASD_H__

#include <errno.h>
#include <stdio.h>

#include "FATFileSystem.h"
#include "LekaInterfaces.h"
#include "SDBlockDevice.h"
#include "mbed.h"

#define BUFFER_MAX_LEN 10

class LekaSD {
  public:
	LekaSD(DigitalOut sd_enable);

	int init();

	void displayProperties();
	void displayReadmeFile();
	void mount();
	void numbersExample();

	void runTest();

  private:
	DigitalOut _sd_enable;
};

#endif	 // __LEKASD_H__
