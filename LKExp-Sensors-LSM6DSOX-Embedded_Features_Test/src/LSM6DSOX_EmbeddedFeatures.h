/**
******************************************************************************
* @file    LSM6DSOX_EmbeddedFeatures.h
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Class to control LSM6DSOX Embedded Features
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LSM6DSOX_EMBEDDEDFEATURES_H__
#define __LSM6DSOX_EMBEDDEDFEATURES_H__

/* Includes ------------------------------------------------------------------*/
#include "CommunicationI2CBase.h"
#include "lsm6dsox_reg.h"
#include "mbed.h"

namespace EmbeddedFeatures {
	/* Defines -------------------------------------------------------------------*/
	/* Enums ---------------------------------------------------------------------*/
    enum class Status : uint8_t {
		OK			  = 0,
		ERROR		  = 1,
		NOT_AVAILABLE = 2,
	};
	/* Structs -------------------------------------------------------------------*/
	/* Unions --------------------------------------------------------------------*/
	/* Class Declaration ---------------------------------------------------------*/

	class LSM6DSOX_EmbeddedFeatures {
	  public:
		LSM6DSOX_EmbeddedFeatures(Communication::I2CBase &component_i2c, PinName pin_interrupt1);

		virtual Status init();
		virtual Status disableI3C();
		virtual Status getID(uint8_t &id);

		virtual Status restoreDefaultConfiguration();

		//temporary, to simplify developpement
		// TODO erase this once no more in use
		virtual stmdev_ctx_t* TMP_getIoFunc();

		// virtual Status getData(std::array<uint8_t, 8> &data);

		// virtual Status enableInterruptRequest();
		// virtual Status disableInterruptRequest();

		// virtual Status enableActivityInterrupt();
		// virtual Status disableActivityInterrupt();

		// virtual Status getInterruptStatus();

		// virtual Status readInterrupt(uint8_t &interrupt_status);
		// virtual Status attachInterrupt(Callback<void()> func);

	  protected:
		static int32_t ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
									uint16_t number_bytes_to_write);
		static int32_t ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
								   uint16_t number_bytes_to_read);

	  private:
		Communication::I2CBase &_lsm6dsox_component_i2c;
		stmdev_ctx_t _register_io_function;
		PinName _mcu_pin_interrupt1;
		InterruptIn _lsm6dsox_interrupt1;
	};
}	// namespace LSM6DSOX_EmbeddedFeatures

#endif	 // __LSM6DSOX_EMBEDDEDFEATURES_H__