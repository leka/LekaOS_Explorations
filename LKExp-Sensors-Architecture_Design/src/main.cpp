// Leka Project - LekaOS_Explorations
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "Accelerometer.h"
#include "Config.h"
#include "Gyroscope.h"
#include "LSM6DSOXDriver.h"
#include "mbed.h"

I2C i2c(LSM6DSOX::I2C::SDA, LSM6DSOX::I2C::SCL);

DigitalOut dsox_int1(LSM6DSOX::INT::INT1, 0);

LSM6DSOXDriver dsox(i2c);
// Accelerometer acc(dsox);

int main(void)
{
	rtos::ThisThread::sleep_for(3s);

	uint8_t id;
	if (dsox.getId(id) == Status::SUCCESS) {
		printf("Acc id is %u\n", id);
	} else {
		printf("error id...\n");
	}

	rtos::ThisThread::sleep_for(1s);

	if (dsox.init() == Status::SUCCESS) {
		printf("Init succeeded\n");
	} else {
		printf("error init...\n");
	}

	rtos::ThisThread::sleep_for(1s);

	std::array<mg_t, 3> data {0};
	std::array<int16_t, 3> raw_data {0};

	while (true) {
		if (dsox.getAccelXYZ(data) == Status::SUCCESS) {
			printf("x: %f, y: %f, z: %f\n", data[0], data[1], data[2]);
		} else {
			printf("error data...\n");
		}

		rtos::ThisThread::sleep_for(200ms);
	}

	return 0;
}
