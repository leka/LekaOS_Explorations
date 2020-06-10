/**
******************************************************************************
* @file    DeviceAccelerometerBase.h
* @author  Yann Locatelli
* @brief   Abstract base class for accelerometer device.
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __DEVICEACCELEROMETERBASE_H__
#define __DEVICEACCELEROMETERBASE_H__

/* Includes ------------------------------------------------------------------*/
#include "DeviceBase.h"
#include "mbed.h"

namespace Device {
	/* Defines -------------------------------------------------------------------*/
	/* Enums ---------------------------------------------------------------------*/
	/* Structs -------------------------------------------------------------------*/
	/* Unions --------------------------------------------------------------------*/
	union AccelerometerData {
		std::array<float, 3> data;
		struct {
			float x;
			float y;
			float z;
		} mg;
	};

	/* Class Declaration ---------------------------------------------------------*/
	/**
	 * Abstract base class for accelerometer device.
	 */
	class AccelerometerBase : DeviceBase {
	  public:
		virtual ~AccelerometerBase() {}
	};
}	// namespace Device

#endif	 // __DEVICEACCELEROMETERBASE_H__