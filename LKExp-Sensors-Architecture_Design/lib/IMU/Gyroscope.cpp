// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "Gyroscope.h"

dps_t Gyroscope::getAngularSpeedX()
{
	dps_t speed;
	_gyro.getAngularSpeedX(speed);
	return speed;
}

dps_t Gyroscope::getAngularSpeedY()
{
	dps_t speed;
	_gyro.getAngularSpeedY(speed);
	return speed;
}

dps_t Gyroscope::getAngularSpeedZ()
{
	dps_t speed;
	_gyro.getAngularSpeedZ(speed);
	return speed;
}
