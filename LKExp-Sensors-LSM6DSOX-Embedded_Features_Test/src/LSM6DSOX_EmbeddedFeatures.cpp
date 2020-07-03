/**
******************************************************************************
* @file    LSM6DSOX_EmbeddedFeatures.cpp
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Class to control LSM6DSOX Embedded Features
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "LSM6DSOX_EmbeddedFeatures.h"

namespace EmbeddedFeatures {
	/* Class Implementation ------------------------------------------------------*/

	/**
	 * @brief Construct a new LSM6DSOX_EmbeddedFeatures object
	 * 
	 * @param component_i2c I2C communication interface
	 * @param pin_interrupt1 Device pin to which the LSM6DSOX INT1 pin is connected
	 * @param pin_interrupt2 Device pin to which the LSM6DSOX INT2 pin is connected
	 */
	LSM6DSOX_EmbeddedFeatures::LSM6DSOX_EmbeddedFeatures(Communication::I2CBase &component_i2c,
												   PinName pin_interrupt1)
		: _lsm6dsox_component_i2c(component_i2c),
		  _mcu_pin_interrupt1(pin_interrupt1),
		  _lsm6dsox_interrupt1(pin_interrupt1){
		_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
		_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
		_register_io_function.handle	= (void *)this;
	}

	
	Status LSM6DSOX_EmbeddedFeatures::init() {
		return Status::OK;
	}

	/**
	 * @brief Disable I3C interface
	 * 
	 * Disabling I3C on the sensor allows to use the INT1 pin as input while keeping I2C active.
	 * If I3C is not disabled and INT1 pin stops being at 0, the sensor will shut down its I2C interface.
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_EmbeddedFeatures::disableI3C() {
		/* Disable I3C interface */
		if (lsm6dsox_i3c_disable_set(&_register_io_function, LSM6DSOX_I3C_DISABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

	/**
	 * @brief Get the ID of the LSM6DSOX sensor connected 
	 * 
	 * @param id where to store the ID
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_EmbeddedFeatures::getID(uint8_t &id) {
		uint8_t p_data = 0;
		if (lsm6dsox_device_id_get(&_register_io_function, &p_data) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		id = p_data;
		
		return Status::OK;
	}

	/**
	 * @brief Restores the default values of all the control registers of the sensor
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_EmbeddedFeatures::restoreDefaultConfiguration()
	{
		uint8_t rst;
		if (lsm6dsox_reset_set(&_register_io_function, PROPERTY_ENABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		do {
			if (lsm6dsox_reset_get(&_register_io_function, &rst) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
			}
		} while (rst);

		return Status::OK;
	}


	//this is a temporary method to ease up developpement
	// TODO erase once no more in use 
	/**
	 * @brief Allow to recover the Register IO function from the inner class
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	stmdev_ctx_t* LSM6DSOX_EmbeddedFeatures::TMP_getIoFunc()
	{
		return &_register_io_function;
	}

	
	/**
	 * @brief  Function used by driver to write data.
	 * @param  handle current object
	 * @param  write_address address of the register to write on
	 * @param  p_buffer contains data to write
	 * @param  number_bytes_to_write number of data to write in bytes
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_EmbeddedFeatures::ptr_io_write(void *handle, uint8_t write_address,
												 uint8_t *p_buffer,
												 uint16_t number_bytes_to_write) {
		return (int32_t)((LSM6DSOX_EmbeddedFeatures *)handle)
			->_lsm6dsox_component_i2c.write(write_address, number_bytes_to_write, p_buffer);
	}

	/**
	 * @brief  Function used by driver to read data.
	 * @param  handle current object
	 * @param  read_address address of the register to read on
	 * @param  p_buffer contains data to read
	 * @param  number_bytes_to_write number of data to read in bytes
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_EmbeddedFeatures::ptr_io_read(void *handle, uint8_t read_address,
												uint8_t *p_buffer, uint16_t number_bytes_to_read) {
		return (int32_t)((LSM6DSOX_EmbeddedFeatures *)handle)
			->_lsm6dsox_component_i2c.read(read_address, number_bytes_to_read, p_buffer);
	}
}	// namespace EmbeddedFeatures
