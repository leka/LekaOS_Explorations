/**
******************************************************************************
* @file    gyroscope.cpp
* @author  Yann Locatelli
* @brief   Implement gyroscope of LSM6DSOX for Leka.
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __GYROSCOPE_H__
#define __GYROSCOPE_H__

/* Includes ------------------------------------------------------------------*/

#include <assert.h>
#include "mbed.h"
#include "lsm6dsox_reg.h"

/* Defines -------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/

enum class GyroStatus : int32_t {
	OK = 0,
	ERROR = -1,
};

enum class GyroPowerMode {
	HIGH_PERFORMANCE,
	NORMAL,
	LOW,
	//ULTRA_LOW,
    OFF,
};

enum class GyroTransmissionStatus : uint8_t {
	OK = 0,
	ERROR = 1,
	overflow = 2,
};

/* Class Declaration ---------------------------------------------------------*/

/**
 * Abstract class of an LSM6DSOX Inertial Measurement Unit (IMU) 6 axes
 * sensor.
 */

class Gyroscope
{
	public:
		Gyroscope(I2C *i2c, uint8_t address=LSM6DSOX_I2C_ADD_L);
		GyroStatus init(void *init);
		GyroStatus read_id(uint8_t *id);
		GyroStatus get_status(GyroPowerMode *powerMode, float *dataRate, uint16_t *fullScale);
		GyroStatus get_int_status(uint8_t *dataReady);
		GyroStatus get_data(float *dps_X, float *dps_Y, float *dps_Z);

	/**
	 * @brief Utility function to read data.
	 * @param  pBuffer: pointer to data to be read.
	 * @param  RegisterAddr: specifies internal address register to be read.
	 * @param  NumByteToRead: number of bytes to be read.
	 * @retval 0 if ok, an error code otherwise.
	 */
	uint8_t io_read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
	{
		//if (!_i2c) return Transmission_ERROR;

		int ret;

		/* Send device address, with no STOP condition */
		ret = _i2c->write(_address, (const char*)&RegisterAddr, 1, true);
		if(!ret) {
			/* Read data, with STOP condition  */
			ret = _i2c->read(_address, (char*)pBuffer, NumByteToRead, false);
		}
		
		if(ret) return (uint8_t)GyroTransmissionStatus::ERROR;
		return (uint8_t)GyroTransmissionStatus::OK;
	}

	/**
	 * @brief Utility function to write data.
	 * @param  pBuffer: pointer to data to be written.
	 * @param  RegisterAddr: specifies internal address register to be written.
	 * @param  NumByteToWrite: number of bytes to write.
	 * @retval 0 if ok, an error code otherwise.
	 */
	uint8_t io_write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
	{
		//if (!_i2c) return Transmission_ERROR;

		int ret;
		uint8_t tmp[TEMP_BUF_SIZE];

		if(NumByteToWrite >= TEMP_BUF_SIZE) return (uint8_t)GyroTransmissionStatus::overflow;

		/* First, send device address. Then, send data and STOP condition */
		tmp[0] = RegisterAddr;
		memcpy(tmp+1, pBuffer, NumByteToWrite);

		ret = _i2c->write(_address, (const char*)tmp, NumByteToWrite+1, false);

		if(ret) return (uint8_t)GyroTransmissionStatus::ERROR;
		return (uint8_t)GyroTransmissionStatus::OK;
	}

	private:
		I2C *_i2c;
		uint8_t _address;

		stmdev_ctx_t _reg_ctx;
		static const unsigned int TEMP_BUF_SIZE = 32;

		lsm6dsox_md_t _status;
		lsm6dsox_all_sources_t _int_status;
		lsm6dsox_data_t _data;

};

#ifdef __cplusplus
 extern "C" {
#endif
int32_t LSM6DSOX_gyro_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
int32_t LSM6DSOX_gyro_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );
#ifdef __cplusplus
	}
#endif

#endif // __GYROSCOPE_H__
