// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_ACCELEROMETER_BASE_H_
#define _LEKA_OS_IMU_ACCELEROMETER_BASE_H_

#include "Status.h"
#include "mbed.h"

//
// MARK: - Type definitions
//

using mg_t = float;

class AccelerometerDriverBase
{
  public:
	//
	// MARK: - Virtual methods
	//

	virtual status_t getAccelX(mg_t &) = 0;
	virtual status_t getAccelY(mg_t &) = 0;
	virtual status_t getAccelZ(mg_t &) = 0;

	virtual status_t getAccelXYZ(std::array<mg_t, 3> &) = 0;
};

#endif	 // _LEKA_OS_IMU_ACCELEROMETER_BASE_H_
