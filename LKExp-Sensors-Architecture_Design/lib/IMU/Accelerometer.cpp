// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "Accelerometer.h"

mg_t Accelerometer::getAccelX()
{
	mg_t val;
	auto ret = _acc.getAccelX(val);
	return ret == Status::SUCCESS ? val : 0;
}

mg_t Accelerometer::getAccelY()
{
	mg_t val;
	auto ret = _acc.getAccelY(val);
	return ret == Status::SUCCESS ? val : 0;
}

mg_t Accelerometer::getAccelZ()
{
	mg_t val;
	auto ret = _acc.getAccelZ(val);
	return ret == Status::SUCCESS ? val : 0;
}
