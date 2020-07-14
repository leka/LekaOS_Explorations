#include "LekaSpeaker.h"

namespace LekaSpeakerNS {
	volatile int _time_counter = 0;
	void counter() { _time_counter++; }
}	// namespace LekaSpeakerNS

LekaSpeaker::LekaSpeaker(AnalogOut &interface, PinName enable)
	: _interface(interface), _enable(enable) {
	_enable = 0;
}

void LekaSpeaker::init() {
	for (int k = 0; k < _sample_size; k++) {
		_analog_out_data[k] =
			((1.0 + sin((float(k) / (float)_sample_size * 6.28318530717959))) / 2.0) * _volume_max;
	}
	_enable = 1;
}

void LekaSpeaker::play440(int duration_sec) { playFrequency(440.0, duration_sec); }

void LekaSpeaker::playFrequency(uint16_t freq, int duration_sec) {
	LekaSpeakerNS::_time_counter = 0;
	_ticker.attach(&LekaSpeakerNS::counter, 1.0);

	uint8_t i		   = 0;
	uint32_t time_step = (uint32_t)(
		1000000000.0 / ((float)freq * (float)_sample_size));   // second value is Hz. Used in ns
	while (LekaSpeakerNS::_time_counter < duration_sec) {
		_interface = _analog_out_data[i];
		i		   = (i + 1) & 0x03F;
		wait_ns(time_step);
	}

	_ticker.detach();
}

void LekaSpeaker::runTest(int duration_sec) {
	printf("\nTest of speakers (from MCU)!\n");
	init();
	play440(duration_sec);

	// 20 * multiplicator^duration ~= 20000 //multiplicator(30s) = 1.258
	for (int f = 20; f < 20000; f *= 1.258) { playFrequency(f, 1); }
	printf("\n");
}
