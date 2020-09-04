
#include "LekaScreen.h"

DigitalOut backlight(PB_14, 1);

namespace LekaScreenNS {
	volatile int _time_counter = 0;
	void counter() { _time_counter++; }
}	// namespace LekaScreenNS

LekaScreen::LekaScreen() {}

void LekaScreen::init() {
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	// BSP_LCD_SelectLayer(0);

	BSP_LCD_Clear(LCD_COLOR_WHITE);
}

void LekaScreen::fadingAnimation(int duration_sec){
	uint32_t col = 0xFF000000;
	uint32_t r	 = 0xFF;
	uint32_t g	 = 0x00;
	uint32_t b	 = 0x00;

	LekaScreenNS::_time_counter = 0;
	_ticker.attach(&LekaScreenNS::counter, 1.0);
	
    while(LekaScreenNS::_time_counter < duration_sec){
		if (r > 0 && b == 0) {
			r--;
			g++;
		} else if (g > 0 && r == 0) {
			g--;
			b++;
		} else if (b > 0 && g == 0) {
			r++;
			b--;
		}
		col = 0xFF000000 + (r << 16) + (g << 8) + b;
		// printf("%x\n\r", col);
		BSP_LCD_Clear(col);
		// HAL_Delay(1);
    }
}

void LekaScreen::runTest(int duration_sec) {
	printf("\nTest of screen!\n");
    init();
    fadingAnimation(duration_sec);

	printf("\n");
}
