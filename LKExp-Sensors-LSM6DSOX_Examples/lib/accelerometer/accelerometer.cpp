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
	///* Output data rate selection - power down. */
	//if (lsm6dsox_xl_data_rate_set(&_reg_ctx, LSM6DSOX_XL_ODR_OFF) != LSM6DSOX_OK)
	//{
	//	return LSM6DSOX_ERROR;
	//}
	
	///* Full scale selection. */
	//if (lsm6dsox_xl_full_scale_set(&_reg_ctx, LSM6DSOX_2g) != LSM6DSOX_OK)
	//{
	//	return LSM6DSOX_ERROR;
	//}
	
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

int32_t LSM6DSOX_acc_io_write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite)
{
  return ((Accelerometer *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

int32_t LSM6DSOX_acc_io_read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead)
{
  return ((Accelerometer *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
