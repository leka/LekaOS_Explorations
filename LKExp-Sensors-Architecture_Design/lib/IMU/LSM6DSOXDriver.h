// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_LSM6DSOX_DRIVER_H_
#define _LEKA_OS_IMU_LSM6DSOX_DRIVER_H_

#include <array>

#include "AccelerometerDriverBase.h"
#include "GyroscopeDriverBase.h"
#include "Status.h"
#include "lsm6dsox_reg.h"
#include "mbed.h"

class LSM6DSOXDriver : public AccelerometerDriverBase, public GyroscopeDriverBase
{
  public:
	//
	// MARK: - Enums
	//
	// TODO: simplify the power modes to low, normal and high + turn off if needed
	enum class PowerMode {
		Off,
		UltraLow,
		Low,
		Normal,
		High,
	};

	enum class Range {
		_2G	 = (lsm6dsox_fs_xl_t)lsm6dsox_fs_xl_t::LSM6DSOX_2g,
		_4G	 = (lsm6dsox_fs_xl_t)lsm6dsox_fs_xl_t::LSM6DSOX_4g,
		_8G	 = (lsm6dsox_fs_xl_t)lsm6dsox_fs_xl_t::LSM6DSOX_8g,
		_16G = (lsm6dsox_fs_xl_t)lsm6dsox_fs_xl_t::LSM6DSOX_16g,
	};

	enum class Rate {
		OFF		= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_OFF,
		_1H6	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_1Hz6,
		_12Hz5	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_12Hz5,
		_26Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_26Hz,
		_52Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_52Hz,
		_104Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_104Hz,
		_208Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_208Hz,
		_417Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_417Hz,
		_833Hz	= (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_833Hz,
		_1667Hz = (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_1667Hz,
		_3333Hz = (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_3333Hz,
		_6667Hz = (lsm6dsox_odr_xl_t)lsm6dsox_odr_xl_t::LSM6DSOX_XL_ODR_6667Hz,
	};

	//
	// MARK: - Type definitions
	//

	using rate_t				= Rate;
	using range_t				= Range;
	using power_mode_t			= PowerMode;
	using conversion_function_t = float (*)(int16_t);

	//
	// MARK: - Constructor & Destructor
	//

	LSM6DSOXDriver(I2C &i2c);

	//
	// MARK: - Device functions
	//

	status_t init();
	status_t getId(uint8_t &id);
	status_t setPowerMode(power_mode_t);
	status_t getPowerMode(power_mode_t);

	//
	// MARK: - Accelerometer Driver Base functions
	//

	virtual status_t setDataRate(rate_t) final;
	virtual status_t getDataRate(rate_t &) final;

	virtual status_t setDataRange(range_t) final;
	virtual status_t getDataRange(range_t &) final;

	virtual status_t getAccelX(mg_t &) final;
	virtual status_t getAccelY(mg_t &) final;
	virtual status_t getAccelZ(mg_t &) final;

	status_t getRawAccelXYZ(std::array<int16_t, 3> &);
	virtual status_t getAccelXYZ(std::array<mg_t, 3> &) final;

	//
	// MARK: - Gyroscope Driver Base functions
	//

	virtual status_t getAngularSpeedX(dps_t &) final;
	virtual status_t getAngularSpeedY(dps_t &) final;
	virtual status_t getAngularSpeedZ(dps_t &) final;

	virtual status_t getAngularSpeedXYZ(std::array<dps_t, 3> &) final;

  private:
	//
	// MARK: - Member variables
	//

	I2C &_i2c;
	uint8_t _address = LSM6DSOX_I2C_ADD_L;

	static const int kBufferSize			 = 32;
	std::array<uint8_t, kBufferSize> _buffer = {0};

	// TODO: create a dsox config with the address, accel range, gyro range, accel rate, gyro rate, powermodes, etc.

	//
	// MARK: - I2C Read/Write methods & pointers
	//

	int read(uint8_t register_address, uint8_t *pBuffer, uint16_t number_bytes_to_read);
	int write(uint8_t register_address, uint8_t *pBuffer, uint16_t number_bytes_to_write);

	stmdev_ctx_t _register_io_function;
	static int32_t ptr_io_write(void *handle, uint8_t register_address, uint8_t *p_buffer,
								uint16_t number_bytes_to_write);
	static int32_t ptr_io_read(void *handle, uint8_t register_address, uint8_t *p_buffer,
							   uint16_t number_bytes_to_read);

	//
	// MARK: - General private methods
	//

	conversion_function_t getAccelConversionFunction();
};

#endif	 // _LEKA_OS_IMU_LSM6DSOX_DRIVER_H_d
