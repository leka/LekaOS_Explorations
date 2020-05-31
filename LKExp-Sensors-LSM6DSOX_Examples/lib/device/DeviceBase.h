/**
******************************************************************************
* @file    DeviceBase.h
* @author  Yann Locatelli
* @brief   Abstract base class for any device (sensor, actuator, ...)
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __DEVICEBASE_H__
#define __DEVICEBASE_H__

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"

namespace Device {
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
	/**
	 * Abstract base class for any device.
	 */
	class DeviceBase {
	  public:
		virtual ~DeviceBase() {}
		virtual Status init() = 0;

		virtual Status enable()	 = 0;
		virtual Status disable() = 0;

		virtual Status getData(std::array<float, 3> &data) = 0;
	};
}	// namespace Device

#endif	 // __DEVICEBASE_H__