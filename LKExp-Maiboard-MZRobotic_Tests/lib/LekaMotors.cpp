#include "LekaMotors.h"

namespace LekaMotorsNS {
	volatile int _time_counter = 0;
	void counter() { _time_counter++; }
}	// namespace LekaMotorsNS

LekaMotors::LekaMotors(PwmOut &l_mot, PwmOut &r_mot, PinName in1, PinName in2, PinName in3,
					   PinName in4)
	: _l_mot(l_mot), _r_mot(r_mot), _in1(in1), _in2(in2), _in3(in3), _in4(in4) {}

void LekaMotors::init(float period) {
	_in1 = 1;
	_in2 = 0;
	_in3 = 1;
	_in4 = 0;

	_l_mot.period_us(1000000.0 / period);
	_r_mot.period_us(1000000.0 / period);
}

void LekaMotors::setSpeed(float ratio, uint8_t direction) {
	_in1 = (direction == 0) ? 1 : 0;
	_in2 = (direction == 0) ? 0 : 1;
	_in3 = (direction == 0) ? 1 : 0;
	_in4 = (direction == 0) ? 0 : 1;

	_l_mot = ratio / 2.0;
	_r_mot = ratio / 2.0;
}

void LekaMotors::runTest(int duration_sec) {
	printf("\nTest of motors!\n");
	init();

	LekaMotorsNS::_time_counter = 0;
	_ticker.attach(&LekaMotorsNS::counter, 1.0);

	while (LekaMotorsNS::_time_counter < duration_sec) {
		setSpeed(1.0f, 0);
		wait_us(0.5);
	}

	LekaMotorsNS::_time_counter = 0;
	float slope = 1.0f / (float)duration_sec;

	while (LekaMotorsNS::_time_counter < duration_sec) {
		setSpeed(1.0f - (float)slope * LekaMotorsNS::_time_counter, 1);
		wait_us(0.5);
	}

	setSpeed(0, 1);
	_ticker.detach();

	printf("\n");
}