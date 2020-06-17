#include "mbed.h"
#include "include/LekaLCD.h"

DigitalOut led(LED2);


int main(void) {
    printf("------Programm starting--------\n\r");

    LekaLCD lcd;

    // initialize and select layer 0
    lcd.LTDC_LayerInit(0);
    lcd.setActiveLayer(0);
    // clear layer 0 in yellow
    lcd.clear(0xffffff00);

    // initialize and select layer 1
    lcd.LTDC_LayerInit(1);
    lcd.setActiveLayer(1);
    // draw a white 400x200 rectangle at position (100,50)
	lcd.fillRect(100, 50, 400, 200, 0xffffffff);

	while (true) {
		led = !led;
        HAL_Delay(500);
		//rtos::ThisThread::sleep_for(500ms); // DOESNT WORK (mess up with LCD synchronization)

        /*if(led.read() == 0) {
            lcd.turnOff();
        }
        else {
            lcd.turnOn();
        }*/
	}

	return 0;
}
