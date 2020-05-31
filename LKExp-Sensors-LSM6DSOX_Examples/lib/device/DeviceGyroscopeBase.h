/**
******************************************************************************
* @file    DeviceGyroscopeBase.h
* @author  Yann Locatelli
* @brief   Abstract base class for gyroscope device.
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __DEVICEGYROSCOPEBASE_H__
#define __DEVICEGYROSCOPEBASE_H__

/* Includes ------------------------------------------------------------------*/
#include "DeviceBase.h"
#include "mbed.h"

namespace Device {
	/* Defines -------------------------------------------------------------------*/
	/* Enums ---------------------------------------------------------------------*/
	/* Structs -------------------------------------------------------------------*/
	/* Unions --------------------------------------------------------------------*/
	union GyroscopeData {
		std::array<float, 3> data;
		struct {
			float x;
			float y;
			float z;
		} mdps;
	};

	/* Class Declaration ---------------------------------------------------------*/
	/**
	 * Abstract base class for gyroscope device.
	 */
	class GyroscopeBase : DeviceBase {
	  public:
		virtual ~GyroscopeBase() {}
	};
}	// namespace Device

#endif	 // __DEVICEGYROSCOPEBASE_H__