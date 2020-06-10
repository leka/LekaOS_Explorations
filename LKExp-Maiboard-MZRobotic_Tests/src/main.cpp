#include "LekaBrightness.h"
#include "LekaInterfaces.h"
#include "LekaMicrophone.h"
#include "mbed.h"

LekaMicrophone lekaMicrophone(microphone);
LekaBrightness lekaBrightness(brightness);

int main(void) {
	printf("\nStarting a new run!\r\n\n");

	lekaMicrophone.runTest();
	lekaBrightness.runTest();

	return 0;
}
