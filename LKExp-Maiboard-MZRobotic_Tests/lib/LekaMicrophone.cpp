#include "LekaMicrophone.h"

LekaMicrophone::LekaMicrophone(AnalogIn &interface) : _interface(interface) {}

float LekaMicrophone::readValuePercentage() { return _interface.read() * 100.0f; }
uint16_t LekaMicrophone::readValueNormalized() { return _interface.read_u16(); }

void LekaMicrophone::runTest(int n) {
	printf("\nTest of microphone!\n");

	for (int i; i < n; i++) {
		printf("percentage: %3.3f%%\n", readValuePercentage());
		printf("normalized: 0x%04X \n", readValueNormalized());
	}

	printf("\n");
}