#include "mbed.h"
#include "include/LekaLCD.h"

DigitalOut led(LED2);

// basic hello woorld program with mbed on F769NI Disco
int main(void) {
    printf("------Programm starting--------\n\r");
	LekaLCD lcd;
    
    lcd.setActiveLayer(0);

    lcd.clear(0xff000080);  // not working for some reason ...
	
	//lcd.fillRect(0, 0, 400, 200, 0xffff00ff);
    //lcd.drawPixel(400, 200, 0x00000000);
	
	while (true) {
		led = !led;
        HAL_Delay(500);
		//rtos::ThisThread::sleep_for(100);
	}

	return 0;
}
