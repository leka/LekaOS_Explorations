// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_GYROSCOPE_H_
#define _LEKA_OS_IMU_GYROSCOPE_H_

#include "GyroscopeDriverBase.h"
#include "mbed.h"

class Gyroscope
{
  public:
	Gyroscope(GyroscopeDriverBase &gyro) : _gyro(gyro) {}

	dps_t getAngularSpeedX();
	dps_t getAngularSpeedY();
	dps_t getAngularSpeedZ();

  private:
	GyroscopeDriverBase &_gyro;
};

#endif	 // _LEKA_OS_IMU_GYROSCOPE_H_
