#include "mbed.h"

#define USE_EVENTQUEUE 0

const uint32_t TIMEOUT_MS = 5000;
InterruptIn button(BUTTON1);
bool button_pressed = false;

#if USE_EVENTQUEUE
EventQueue queue;
#else
Ticker kicker;
#endif

void pushed()
{
	button_pressed = true;
}

void released()
{
	button_pressed = false;
}

void watchdogKick()
{
	Watchdog::get_instance().kick();
}

int main()
{
	printf("\r\n\r\nStarting a new run.\r\n");

	button.rise(&pushed);
	button.fall(&released);
	printf("Press BUTTON1 to switch between Infinite loop (button released) and Fatal error (button pressed)\n");
	printf("Wait 5s to make your choice...\n");

	ThisThread::sleep_for(5s);

	printf("You choosed ");
	if (button_pressed) {
		printf("Fatal error\n");
	} else {
		printf("Infinite loop\n");
	}
	printf("\r\nSet watchdog timeout to %lums\r\n", TIMEOUT_MS);
	Watchdog &watchdog = Watchdog::get_instance();
	watchdog.start(TIMEOUT_MS);

#if USE_EVENTQUEUE
	queue.call_every(1, &watchdogKick);
	queue.dispatch();
#else
	kicker.attach_us(watchdogKick, 1000);
#endif

	if (button_pressed) {
		error("This is a fatal error\n");
	} else {
		while (1) { printf("To Infinity and Beyond!! "); }
	}
	return 0;
}
