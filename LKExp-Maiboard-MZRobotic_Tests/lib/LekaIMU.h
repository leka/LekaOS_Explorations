/**
******************************************************************************
* @file    LekaIMU.h
* @author  Yann Locatelli
* @brief   IMU operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAIMU_H__
#define __LEKAIMU_H__

#include "lsm6dsox_reg.h"
#include "mbed.h"

class LekaIMU {
  public:
	LekaIMU(I2C &interface, PinName pin_interrupt, uint8_t address = LSM6DSOX_I2C_ADD_L);
	void init();
	uint8_t getId();
	void getData(std::array<float, 3> &xl_data, std::array<float, 3> &gy_data);

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
	PinName _lsm6dsox_interrupt;
	stmdev_ctx_t _register_io_function;
	lsm6dsox_md_t _config;
	const uint16_t kBufferSize = 32;
};

#endif	 // __LEKAIMU_H__
