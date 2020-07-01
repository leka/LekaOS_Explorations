#include "LekaBrightness.h"

LekaBrightness::LekaBrightness(AnalogIn &interface) : _interface(interface) {}

float LekaBrightness::readValuePercentage() { return _interface.read() * 100.0f; }
uint16_t LekaBrightness::readValueNormalized() { return _interface.read_u16(); }

void LekaBrightness::runTest(int n) {
	printf("\nTest of brightness sensor!\n");

	for (int i=0; i < n; i++) {
		printf("percentage: %3.3f%%\n", readValuePercentage());
		printf("normalized: 0x%04X \n", readValueNormalized());
	}
	printf("\n");
}