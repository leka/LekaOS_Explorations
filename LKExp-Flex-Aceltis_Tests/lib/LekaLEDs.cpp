#include "LekaLEDs.h"

LekaLEDs::LekaLEDs(SPI &leds_ears, SPI &leds_belt) : _leds_ears(leds_ears), _leds_belt(leds_belt) {}

void LekaLEDs::init()
{
	return;
}

void color_led(SPI &leds, int r, int g, int b, int n_leds)
{
	leds.write(0x00);
	leds.write(0x00);
	leds.write(0x00);
	leds.write(0x00);

	for (int i = 0; i < n_leds; i++) {
		leds.write(0xFF);
		leds.write(r);
		leds.write(g);
		leds.write(b);
	}

	leds.write(0x00);
	leds.write(0x00);
	leds.write(0x00);
	leds.write(0x00);
}

void LekaLEDs::runTest(int n_repetition)
{
	printf("\nTest of LEDs!\n");
	init();

	for (int i = 0; i < n_repetition; i++) {
		color_led(_leds_ears, 0xFF, 0xFF, 0xFF, n_leds_ears);
		color_led(_leds_belt, 0xFF, 0xFF, 0xFF, n_leds_belt);
		rtos::ThisThread::sleep_for(1s);
		color_led(_leds_ears, 0xFF, 0x00, 0x00, n_leds_ears);
		color_led(_leds_belt, 0xFF, 0x00, 0x00, n_leds_belt);
		rtos::ThisThread::sleep_for(1s);
		color_led(_leds_ears, 0x00, 0xFF, 0x00, n_leds_ears);
		color_led(_leds_belt, 0x00, 0xFF, 0x00, n_leds_belt);
		rtos::ThisThread::sleep_for(1s);
		color_led(_leds_ears, 0x00, 0x00, 0xFF, n_leds_ears);
		color_led(_leds_belt, 0x00, 0x00, 0xFF, n_leds_belt);
		rtos::ThisThread::sleep_for(1s);
		color_led(_leds_ears, 0x00, 0x00, 0x00, n_leds_ears);
		color_led(_leds_belt, 0x00, 0x00, 0x00, n_leds_belt);
		rtos::ThisThread::sleep_for(1s);
	}

	printf("\n");
}
