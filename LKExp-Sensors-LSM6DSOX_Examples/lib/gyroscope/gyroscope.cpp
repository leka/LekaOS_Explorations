/**
******************************************************************************
* @file    gyroscope.h
* @author  Yann Locatelli
* @brief   Implement gyroscope of LSM6DSOX for Leka.
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "gyroscope.h"

/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param i2c object which handles the I2C peripheral
 * @param address the address of the component
 */
Gyroscope::Gyroscope(I2C *i2c, uint8_t address) : _i2c(i2c), _address(address)
{
	assert (i2c);

	_reg_ctx.write_reg = LSM6DSOX_gyro_io_write;
	_reg_ctx.read_reg = LSM6DSOX_gyro_io_read;
	_reg_ctx.handle = (void *)this;
}

/**
 * @brief  Initializing the component
 * @param  init pointer to device specific initalization structure
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXGyroStatusTypeDef Gyroscope::init(void *init)
{
	/* Initialize the device for driver usage */
	if (lsm6dsox_init_set(&_reg_ctx, LSM6DSOX_DRV_RDY) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}

	//Get current status
	if (lsm6dsox_mode_get(&_reg_ctx, NULL, &_status) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}

	/* Replace output data rate selection and full scale selection. */
	_status.ui.gy.odr = _status.ui.gy.LSM6DSOX_GY_UI_52Hz_LP;
	_status.ui.gy.fs  = _status.ui.gy.LSM6DSOX_GY_UI_500dps;
	if (lsm6dsox_mode_set(&_reg_ctx, NULL, &_status) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}
	
	return LSM6DSOX_Gyro_OK;
}

/**
 * @brief  Read component ID
 * @param  id the WHO_AM_I value
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXGyroStatusTypeDef Gyroscope::read_id(uint8_t *id)
{
	if (lsm6dsox_device_id_get(&_reg_ctx, id) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}

	return LSM6DSOX_Gyro_OK;
}

/**
 * @brief  Read component status
 * @param  powerMode is the power mode
 * @param  dataRate is output data range in float
 * @param  fullScale is full scal in hexadecimal value
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXGyroStatusTypeDef Gyroscope::get_status(PowerModeGyro *powerMode, float *dataRate, uint16_t *fullScale)
{
	LSM6DSOXGyroStatusTypeDef success = LSM6DSOX_Gyro_OK;
	if (lsm6dsox_mode_get(&_reg_ctx, NULL, &_status) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}

	switch((_status.ui.gy.odr & 0x0F))
	{
		case 0:
			*dataRate = 0;
			break;
		case 1:
			*dataRate = 12.5f;
			break;
		case 2:
			*dataRate = 26.0f;
			break;
		case 3:
			*dataRate = 52.0f;
			break;
		case 4:
			*dataRate = 104.0f;
			break;
		case 5:
			*dataRate = 208.0f;
			break;
		case 6:
			*dataRate = 416.0f;
			break;
		case 7:
			*dataRate = 833.0f;
			break;
		case 8:
			*dataRate = 1667.0f;
			break;
		case 9:
			*dataRate = 3333.0f;
			break;
		case 10:
			*dataRate = 6667.0f;
			break;
//		case 11:
//			*dataRate = 1.6f;
//			break;
		default:
			success = LSM6DSOX_Gyro_ERROR;
			break;
	}
	if (success == LSM6DSOX_Gyro_ERROR)
	{
		return success;
	}


	*powerMode 	= (*dataRate == 0)						?	Power_Off_Gyro
				: (not(_status.ui.gy.odr & 0x10) >> 4) 	? 	High_Performance_Gyro 
				: (*dataRate > 100.0f) 					? 	Normal_Power_Gyro 
				: 											Low_Power_Gyro;

	switch((_status.ui.gy.fs))
	{
		case 0:
			*fullScale = 250;
			break;
		case 1:
			*fullScale = 125;
			break;
		case 2:
			*fullScale = 500;
			break;
		case 4:
			*fullScale = 1000;
			break;
		case 6:
			*fullScale = 2000;
			break;
		default:
			success = LSM6DSOX_Gyro_ERROR;
			break;
	}

	return success;
}

/**
 * @brief  Read component interrupt status
 * @param  dataReady flag
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXGyroStatusTypeDef Gyroscope::get_int_status(uint8_t *dataReady)
{
	if (lsm6dsox_all_sources_get(&_reg_ctx, &_int_status) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}
	*dataReady = _int_status.drdy_g;

	return LSM6DSOX_Gyro_OK;
}

/**
 * @brief  Read component data
 * @param  dps_X data value of angular acceleration on X axis in dps
 * @param  dps_Y data value of angular acceleration on Y axis in dps
 * @param  dps_Z data value of angular acceleration on Z axis in dps
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXGyroStatusTypeDef Gyroscope::get_data(float *dps_X, float *dps_Y, float *dps_Z)
{
	//if (lsm6dsox_data_get(&_reg_ctx, NULL, &_status, &_data) != LSM6DSOX_Gyro_OK) //Lot of mistakes in the driver's function
	//{
	//	return LSM6DSOX_Gyro_ERROR;
	//}
	//*mg_X = _data.ui.gy.dps[0];
	//*mg_Y = _data.ui.gy.dps[1];
	//*mg_Z = _data.ui.gy.dps[2];

	LSM6DSOXGyroStatusTypeDef success = LSM6DSOX_Gyro_OK;
	uint8_t data_raw[6];

	/* Read raw data values. */
	if (lsm6dsox_angular_rate_raw_get(&_reg_ctx, data_raw) != LSM6DSOX_Gyro_OK)
	{
		return LSM6DSOX_Gyro_ERROR;
	}

	float_t (* pConversionFunction) (int16_t raw_value);
	switch(_status.ui.gy.fs)
	{
		case 0:
			pConversionFunction = &lsm6dsox_from_fs250_to_mdps;
			break;
		case 1:
			pConversionFunction = &lsm6dsox_from_fs125_to_mdps;
			break;
		case 2:
			pConversionFunction = &lsm6dsox_from_fs500_to_mdps;
			break;
		case 4:
			pConversionFunction = &lsm6dsox_from_fs1000_to_mdps;
			break;
		case 6:
			pConversionFunction = &lsm6dsox_from_fs2000_to_mdps;
			break;
		default:
			success = LSM6DSOX_Gyro_ERROR;
			break;
	}

	*dps_X = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[1] << 8) | data_raw[0]);
	*dps_Y = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[3] << 8) | data_raw[2]);
	*dps_Z = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[5] << 8) | data_raw[4]);

	return success;
}

int32_t LSM6DSOX_gyro_io_write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite)
{
  return ((Gyroscope *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

int32_t LSM6DSOX_gyro_io_read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead)
{
  return ((Gyroscope *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
