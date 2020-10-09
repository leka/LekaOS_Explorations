/**
******************************************************************************
* @file    LekaUpdater.h
* @author  Yann Locatelli
* @brief   Receive and stock in external flash .bin APPLICATION in APPLICATION mode,
*          Write .bin APPLICATION stocked in external flash in BOOTLOADER mode.
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAUPDATER_H__
#define __LEKAUPDATER_H__

#include "PinNames.h"
#include "QSPI.h"
#include "QspiMemory.h"
#include "mbed.h"

class Updater
{
  public:
	static int receiveFile();
	static void flashQSPI(uint32_t flash_address);
	static bool checkNew();

  private:
};

#endif	 //__LEKAUPDATER_H__