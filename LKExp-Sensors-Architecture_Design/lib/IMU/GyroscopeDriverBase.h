// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_GYROSCOPE_DRIVER_BASE_H_
#define _LEKA_OS_IMU_GYROSCOPE_DRIVER_BASE_H_

#include "Status.h"
#include "mbed.h"

using dps_t = int;

class GyroscopeDriverBase
{
  public:
	virtual status_t getAngularSpeedX(dps_t &) = 0;
	virtual status_t getAngularSpeedY(dps_t &) = 0;
	virtual status_t getAngularSpeedZ(dps_t &) = 0;

	virtual status_t getAngularSpeedXYZ(std::array<dps_t, 3> &) = 0;

	// TODO: maybe be need to get and set data rate/range
	// virtual status_t setDataRate(rate_t)		 = 0;
	// virtual status_t getDataRate(rate_t &) const = 0;

	// virtual status_t setDataRange(range_t)		   = 0;
	// virtual status_t getDataRange(range_t &) const = 0;
};

#endif	 // _LEKA_OS_IMU_GYROSCOPE_DRIVER_BASE_H_
