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

typedef enum
{
	LSM6DSOX_Gyro_OK = 0,
	LSM6DSOX_Gyro_ERROR = -1
} LSM6DSOXGyroStatusTypeDef;

typedef enum
{
	High_Performance_Gyro = 0,
	Normal_Power_Gyro = 1,
	Low_Power_Gyro = 2,
//	Ultra_Low_Power_Gyro = 3,
    Power_Off_Gyro = 4
}PowerModeGyro;

typedef enum
{
	Transmission_Gyro_OK = 0,
	Transmission_Gyro_ERROR = -1,
	Transmission_Gyro_OVERFLOW = -2
} TransmissionGyroStatusTypeDef;

/* Class Declaration ---------------------------------------------------------*/

/**
 * Abstract class of an LSM6DSOX Inertial Measurement Unit (IMU) 6 axes
 * sensor.
 */

class Gyroscope
{
	public:
		Gyroscope(I2C *i2c, uint8_t address=LSM6DSOX_I2C_ADD_L);
		LSM6DSOXGyroStatusTypeDef init(void *init);
		LSM6DSOXGyroStatusTypeDef read_id(uint8_t *id);
		LSM6DSOXGyroStatusTypeDef get_status(PowerModeGyro *powerMode, float *dataRate, uint16_t *fullScale);
		LSM6DSOXGyroStatusTypeDef get_int_status(uint8_t *dataReady);
		LSM6DSOXGyroStatusTypeDef get_data(float *dps_X, float *dps_Y, float *dps_Z);

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
		
		if(ret) return Transmission_Gyro_ERROR;
		return Transmission_Gyro_OK;
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

		if(NumByteToWrite >= TEMP_BUF_SIZE) return Transmission_Gyro_OVERFLOW;

		/* First, send device address. Then, send data and STOP condition */
		tmp[0] = RegisterAddr;
		memcpy(tmp+1, pBuffer, NumByteToWrite);

		ret = _i2c->write(_address, (const char*)tmp, NumByteToWrite+1, false);

		if(ret) return Transmission_Gyro_ERROR;
		return Transmission_Gyro_OK;
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
