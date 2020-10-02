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

#include "QSPI.h"
#include "mbed.h"
// #include "MX25L51245G_config.h"

#define CMD_READ 0x03
#define CMD_WRITE 0x02
#define CMD_ERASE 0x20
#define CMD_RDSR 0x5
#define CMD_WREN 0x6
#define CMD_RSTEN 0x66
#define CMD_RST 0x99
#define STATUS_REG_SIZE 2
#define BIT_WIP 0x1
#define BIT_WEL 0x2

#define BUF_SIZE 12

class QspiMemory
{
  public:
	QspiMemory(PinName qspi_flash_io0 = QSPI_FLASH1_IO0, PinName qspi_flash_io1 = QSPI_FLASH1_IO1,
			   PinName qspi_flash_io2 = QSPI_FLASH1_IO2, PinName qspi_flash_io3 = QSPI_FLASH1_IO3,
			   PinName qspi_flash_sck = QSPI_FLASH1_SCK, PinName qspi_flash_csn = QSPI_FLASH1_CSN);

	bool mem_ready();
	int write_enable();
	int flash_init();
	int sector_erase(unsigned int flash_addr);
	int init();
	int clean_up(uint32_t write_address, uint32_t read_address);
	int ext_flash_write(uint32_t write_address, char tx_buf[], size_t buf_len);
	int ext_flash_read(uint32_t read_address);
	int ext_flash_get_data(uint32_t read_address, char rx_buf[], size_t buf_len);
	void example();

  private:
	QSPI _qspi_device;
};

#endif	 // __LEKAFIRMWARE_H__
