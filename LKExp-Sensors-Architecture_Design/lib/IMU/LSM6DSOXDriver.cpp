// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "LSM6DSOXDriver.h"

//
// MARK: - Constructor, initialization
//

LSM6DSOXDriver::LSM6DSOXDriver(I2C &i2c) : _i2c(i2c)
{
	_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
	_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
	_register_io_function.handle	= (void *)this;
}

status_t LSM6DSOXDriver::init()
{
	// Reset the component
	if (auto ret = lsm6dsox_init_set(&_register_io_function, LSM6DSOX_RESET); ret != 0) {
		// TODO: add debug
		printf("error reset...\n");
		return Status::ERROR;
	}

	// Initialize the component for driver usage
	if (auto ret = lsm6dsox_init_set(&_register_io_function, LSM6DSOX_DRV_RDY); ret != 0) {
		// TODO: add debug
		printf("error driver...\n");
		return Status::ERROR;
	}

	if (auto ret = setPowerMode(PowerMode::Normal); ret != Status::SUCCESS) {
		// TODO: add debug
		printf("error powermode...\n");
		return ret;
	}

	if (auto ret = setDataRate(Rate::_52Hz); ret != Status::SUCCESS) {
		// TODO: add debug
		printf("error data rate...\n");
		return ret;
	}

	if (auto ret = setDataRange(Range::_2G); ret != Status::SUCCESS) {
		// TODO: add debug
		printf("error range...\n");
		return ret;
	}

	return Status::SUCCESS;
}

//
// MARK: - Read & Write functions

//

int LSM6DSOXDriver::read(uint8_t register_address, uint8_t *pBuffer, uint16_t number_bytes_to_read)
{
	// Send component address, with no STOP condition
	int ret = _i2c.write(_address, (const char *)&register_address, 1, true);
	if (ret == 0) {
		// Read data, with STOP condition
		ret = _i2c.read(_address, (char *)pBuffer, number_bytes_to_read, false);
	}

	return ret;
}

int LSM6DSOXDriver::write(uint8_t register_address, uint8_t *pBuffer, uint16_t number_bytes_to_write)
{
	if (number_bytes_to_write > (kBufferSize - 1)) { return 1; };

	_buffer[0] = register_address;	 // First, send register address
	std::copy(pBuffer, (pBuffer + number_bytes_to_write), (_buffer.begin() + 1));

	int ret = _i2c.write(_address, (const char *)_buffer.data(), (number_bytes_to_write + 1), false);

	return ret;
}

int32_t LSM6DSOXDriver::ptr_io_write(void *handle, uint8_t register_address, uint8_t *p_buffer,
									 uint16_t number_bytes_to_write)
{
	return (int32_t)((LSM6DSOXDriver *)handle)->write(register_address, p_buffer, number_bytes_to_write);
}

int32_t LSM6DSOXDriver::ptr_io_read(void *handle, uint8_t register_address, uint8_t *p_buffer,
									uint16_t number_bytes_to_read)
{
	return (int32_t)((LSM6DSOXDriver *)handle)->read(register_address, p_buffer, number_bytes_to_read);
}

status_t LSM6DSOXDriver::setPowerMode(power_mode_t mode)
{
	// ? Is this really power off? The documentation states that the DSOX is "always on"
	// TODO: double check that have on output data rate == 0 really saves power
	if (mode == PowerMode::Off) {
		if (auto ret = lsm6dsox_xl_data_rate_set(&_register_io_function, LSM6DSOX_XL_ODR_OFF); ret != 0) {
			return Status::ERROR;
		}
	} else {
		lsm6dsox_xl_hm_mode_t power_mode;
		switch (mode) {
			case PowerMode::UltraLow:
				power_mode = LSM6DSOX_ULTRA_LOW_POWER_MD;
				break;
			case PowerMode::Low:
				power_mode = LSM6DSOX_LOW_NORMAL_POWER_MD;
				break;
			case PowerMode::Normal:
				power_mode = LSM6DSOX_LOW_NORMAL_POWER_MD;
				break;
			case PowerMode::High:
				power_mode = LSM6DSOX_HIGH_PERFORMANCE_MD;
				break;
			default:
				return Status::ERROR;
		}

		if (auto ret = lsm6dsox_xl_power_mode_set(&_register_io_function, power_mode); ret != 0) {
			return Status::ERROR;
		}

		return Status::SUCCESS;
	}
}

status_t LSM6DSOXDriver::setDataRange(range_t range)
{
	if (auto ret = lsm6dsox_xl_full_scale_set(&_register_io_function, (lsm6dsox_fs_xl_t)range); ret != 0) {
		return Status::ERROR;
	}
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getDataRange(range_t &range)
{
	if (auto ret = lsm6dsox_xl_full_scale_get(&_register_io_function, (lsm6dsox_fs_xl_t *)&range); ret != 0) {
		return Status::ERROR;
	}
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::setDataRate(rate_t rate)
{
	if (auto ret = lsm6dsox_xl_data_rate_set(&_register_io_function, (lsm6dsox_odr_xl_t)rate); ret != 0) {
		return Status::ERROR;
	}
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getDataRate(rate_t &rate)
{
	if (auto ret = lsm6dsox_xl_data_rate_get(&_register_io_function, (lsm6dsox_odr_xl_t *)&rate); ret != 0) {
		return Status::ERROR;
	}
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getId(uint8_t &id)
{
	if (auto ret = lsm6dsox_device_id_get(&_register_io_function, &id); ret != 0) { return Status::ERROR; }
	return Status::SUCCESS;
}

// MARK: - Private functions

// MARK: - Accelerometer Base functions

status_t LSM6DSOXDriver::getAccelX(mg_t &acc)
{
	// TODO: implement function
	acc = 42;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAccelY(mg_t &acc)
{
	// TODO: implement function
	acc = 43;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAccelZ(mg_t &acc)
{
	// TODO: implement function
	acc = 44;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getRawAccelXYZ(std::array<int16_t, 3> &data)
{
	uint8_t buff[6];
	if (auto ret = lsm6dsox_acceleration_raw_get(&_register_io_function, buff); ret != 0) { return Status::ERROR; }

	// Every data is composed of 2 bytes, the first of them is the LSB.
	int16_t x = ((uint16_t)buff[1] << 8) | buff[0];
	int16_t y = ((uint16_t)buff[3] << 8) | buff[2];
	int16_t z = ((uint16_t)buff[5] << 8) | buff[4];

	data = {x, y, z};
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAccelXYZ(std::array<mg_t, 3> &data)
{
	std::array<int16_t, 3> buff {0};
	conversion_function_t convert = getAccelConversionFunction();

	if (getRawAccelXYZ(buff) == Status::SUCCESS) {
		data[0] = convert(buff[0]);
		data[1] = convert(buff[1]);
		data[2] = convert(buff[2]);
		return Status::SUCCESS;
	}

	return Status::ERROR;
}

status_t LSM6DSOXDriver::getAngularSpeedX(dps_t &speed)
{
	dps_t val = 42;
	speed	  = val;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAngularSpeedY(dps_t &speed)
{
	dps_t val = 43;
	speed	  = val;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAngularSpeedZ(dps_t &speed)
{
	dps_t val = 44;
	speed	  = val;
	return Status::SUCCESS;
}

status_t LSM6DSOXDriver::getAngularSpeedXYZ(std::array<dps_t, 3> &speed)
{
	std::array<dps_t, 3> buff {42, 43, 44};
	speed = buff;
	return Status::SUCCESS;
}

LSM6DSOXDriver::conversion_function_t LSM6DSOXDriver::getAccelConversionFunction()
{
	range_t range;
	getDataRange(range);
	switch (range) {
		case Range::_2G:
			return lsm6dsox_from_fs2_to_mg;
		case Range::_4G:
			return lsm6dsox_from_fs4_to_mg;
		case Range::_8G:
			return lsm6dsox_from_fs8_to_mg;
		case Range::_16G:
			return lsm6dsox_from_fs16_to_mg;
	}
}
