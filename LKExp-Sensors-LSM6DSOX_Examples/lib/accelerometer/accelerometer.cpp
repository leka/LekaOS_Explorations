/**
******************************************************************************
* @file    accelerometer.h
* @author  Yann Locatelli
* @brief   Implement accelerometer of LSM6DSOX for Leka.
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "accelerometer.h"

/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param i2c object which handles the I2C peripheral
 * @param address the address of the component
 */
Accelerometer::Accelerometer(I2C *i2c, uint8_t address) : _i2c(i2c), _address(address)
{
	assert (i2c);

	_reg_ctx.write_reg = LSM6DSOX_acc_io_write;
	_reg_ctx.read_reg = LSM6DSOX_acc_io_read;
	_reg_ctx.handle = (void *)this;
}

/**
 * @brief  Initializing the component
 * @param  init pointer to device specific initalization structure
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXAccStatusTypeDef Accelerometer::init(void *init)
{
	/* Initialize the device for driver usage */
	if (lsm6dsox_init_set(&_reg_ctx, LSM6DSOX_DRV_RDY) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}

	//Get current status
	if (lsm6dsox_mode_get(&_reg_ctx, NULL, &_status) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}

	/* Replace output data rate selection and full scale selection. */
	_status.ui.xl.odr = _status.ui.xl.LSM6DSOX_XL_UI_52Hz_LP;
	_status.ui.xl.fs  = _status.ui.xl.LSM6DSOX_XL_UI_4g;
	if (lsm6dsox_mode_set(&_reg_ctx, NULL, &_status) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}
	
	return LSM6DSOX_Acc_OK;
}

/**
 * @brief  Read component ID
 * @param  id the WHO_AM_I value
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXAccStatusTypeDef Accelerometer::read_id(uint8_t *id)
{
	if (lsm6dsox_device_id_get(&_reg_ctx, id) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}

	return LSM6DSOX_Acc_OK;
}

/**
 * @brief  Read component status
 * @param  powerMode is the power mode
 * @param  dataRate is output data range in float
 * @param  fullScale is full scal in hexadecimal value
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXAccStatusTypeDef Accelerometer::get_status(PowerModeAcc *powerMode, float *dataRate, uint16_t *fullScale)
{
	LSM6DSOXAccStatusTypeDef success = LSM6DSOX_Acc_OK;
	if (lsm6dsox_mode_get(&_reg_ctx, NULL, &_status) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}

	switch((_status.ui.xl.odr & 0x0F))
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
		case 11:
			*dataRate = 1.6f;
			break;
		default:
			success = LSM6DSOX_Acc_ERROR;
			break;
	}
	if (success == LSM6DSOX_Acc_ERROR)
	{
		return success;
	}


	*powerMode 	= (*dataRate == 0)						?	Power_Off_Acc
				: ((_status.ui.xl.odr & 0x20) >> 5) 	? 	Ultra_Low_Power_Acc
				: (not(_status.ui.xl.odr & 0x10) >> 4) 	? 	High_Performance_Acc
				: (*dataRate > 100.0f) 					? 	Normal_Power_Acc
				: 											Low_Power_Acc;


	switch((_status.ui.xl.fs))
	{
		case 0:
			*fullScale = 2;
			break;
		case 1:
			*fullScale = 16;
			break;
		case 2:
			*fullScale = 4;
			break;
		case 3:
			*fullScale = 8;
			break;
		default:
			success = LSM6DSOX_Acc_ERROR;
			break;
	}

	return success;
}

/**
 * @brief  Read component interrupt status
 * @param  dataReady flag
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXAccStatusTypeDef Accelerometer::get_int_status(uint8_t *dataReady)
{
	if (lsm6dsox_all_sources_get(&_reg_ctx, &_int_status) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}
	*dataReady = _int_status.drdy_xl;

	return LSM6DSOX_Acc_OK;
}

/**
 * @brief  Read component data
 * @param  mg_X data value of acceleration on X axis in mg
 * @param  mg_Y data value of acceleration on Y axis in mg
 * @param  mg_Z data value of acceleration on Z axis in mg
 * @retval 0 in case of success, an error code otherwise
 */
LSM6DSOXAccStatusTypeDef Accelerometer::get_data(float *mg_X, float *mg_Y, float *mg_Z)
{
	//if (lsm6dsox_data_get(&_reg_ctx, NULL, &_status, &_data) != LSM6DSOX_Acc_OK) //Lot of mistakes in the driver's function
	//{
	//	return LSM6DSOX_Acc_ERROR;
	//}
	//*mg_X = _data.ui.xl.mg[0];
	//*mg_Y = _data.ui.xl.mg[1];
	//*mg_Z = _data.ui.xl.mg[2];

	LSM6DSOXAccStatusTypeDef success = LSM6DSOX_Acc_OK;
	uint8_t data_raw[6];

	/* Read raw data values. */
	if (lsm6dsox_acceleration_raw_get(&_reg_ctx, data_raw) != LSM6DSOX_Acc_OK)
	{
		return LSM6DSOX_Acc_ERROR;
	}

	float_t (* pConversionFunction) (int16_t raw_value);
	switch(_status.ui.xl.fs)
	{
		case 0:
			pConversionFunction = &lsm6dsox_from_fs2_to_mg;
			break;
		case 1:
			pConversionFunction = &lsm6dsox_from_fs16_to_mg;
			break;
		case 2:
			pConversionFunction = &lsm6dsox_from_fs4_to_mg;
			break;
		case 3:
			pConversionFunction = &lsm6dsox_from_fs8_to_mg;
			break;
		default:
			success = LSM6DSOX_Acc_ERROR;
			break;
	}

	*mg_X = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[1] << 8) | data_raw[0]);
	*mg_Y = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[3] << 8) | data_raw[2]);
	*mg_Z = (float)(* pConversionFunction)((uint16_t)((uint16_t)data_raw[5] << 8) | data_raw[4]);

	return success;
}

int32_t LSM6DSOX_acc_io_write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite)
{
  return ((Accelerometer *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

int32_t LSM6DSOX_acc_io_read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead)
{
  return ((Accelerometer *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
