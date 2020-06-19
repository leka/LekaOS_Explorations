// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_MAIN_CONFIG_H_
#define _LEKA_OS_MAIN_CONFIG_H_

#include "mbed.h"

namespace LSM6DSOX {

	namespace I2C {
		constexpr auto SDA(D14);
		constexpr auto SCL(D15);
	}	// namespace I2C

	namespace INT {
		constexpr auto INT1(A5);
	}	// namespace INT

}	// namespace LSM6DSOX

#endif	 // _LEKA_OS_MAIN_CONFIG_H_
