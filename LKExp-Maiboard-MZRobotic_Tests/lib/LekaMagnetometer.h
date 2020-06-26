/**
******************************************************************************
* @file    LekaMagnetometer.h
* @author  Yann Locatelli
* @brief   Magnetometer operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAMAGNETOMETER_H__
#define __LEKAMAGNETOMETER_H__

#include "lsm303agr_reg.h"
#include "mbed.h"

class LekaMagnetometer {
  public:
	LekaMagnetometer(I2C &interface, PinName pin_interrupt, uint8_t address = LSM303AGR_I2C_ADD_MG);
	void init();
	uint8_t getId();
	void getData(std::array<float, 3> &mag_data);

	void runTest(int n_repetition = 1);

	static int32_t ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
								uint16_t number_bytes_to_write);
	static int32_t ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
							   uint16_t number_bytes_to_read);

	int32_t read(uint8_t register_address, uint16_t number_bytes_to_read, uint8_t *p_buffer);
	int32_t write(uint8_t register_address, uint16_t number_bytes_to_write, uint8_t *p_buffer);

  private:
	I2C &_interface;
	uint8_t _address;
	PinName _lsm303agr_interrupt;
	stmdev_ctx_t _register_io_function;
	const uint16_t kBufferSize = 32;
};

#endif	 // __LEKAMAGNETOMETER_H__
