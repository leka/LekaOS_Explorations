// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_IMU_IMU_H_
#define _LEKA_OS_IMU_IMU_H_

#include "Accelerometer.h"
#include "Gyroscope.h"
#include "mbed.h"

class IMU
{
  public:
	IMU(Accelerometer &acc, Gyroscope &gyro) : _acc(acc), _gyro(gyro) {};

	int getYaw();
	int getPitch();
	int getRoll();

  private:
	Accelerometer &_acc;
	Gyroscope &_gyro;
};

#endif	 // _LEKA_OS_IMU_IMU_H_
