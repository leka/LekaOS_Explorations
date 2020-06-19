// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_ACCELEROMETER_H_
#define _LEKA_OS_IMU_ACCELEROMETER_H_

#include "AccelerometerDriverBase.h"
#include "mbed.h"

class Accelerometer
{
  public:
	struct Data {
		mg_t x;
		mg_t y;
		mg_t z;
	};
	using accel_data_t = Data;

	Accelerometer(AccelerometerDriverBase &acc) : _acc(acc) {}

	mg_t getAccelX();
	mg_t getAccelY();
	mg_t getAccelZ();

	accel_data_t getData();

  private:
	AccelerometerDriverBase &_acc;
};

#endif	 // _LEKA_OS_IMU_ACCELEROMETER_H_
