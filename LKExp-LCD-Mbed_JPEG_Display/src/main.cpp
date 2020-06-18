#include "mbed.h"
#include "include/LekaLCD.h"
#include "include/img/leka_eye_alpha.h"
#include "include/img/leka_mouth_alpha.h"

/* Leka face display from embedded images */
int main(void) {
    printf("--------Programm starting--------\n\r");

    LekaLCD lcd;

    uint32_t bg_color = 0xffffffff;
    
    // initialize and select layer 0
    lcd.LTDC_LayerInit(0);
    //lcd.LTDC_LayerInit(1);
    lcd.setActiveLayer(0);
    // clear layer 0 in white
    lcd.clear(bg_color);

    lcd.drawImage((uint32_t)IMAGE_DATA_EYE,  100, 100, IMAGE_WIDTH_EYE, IMAGE_HEIGHT_EYE);
    lcd.drawImage((uint32_t)IMAGE_DATA_EYE,  500, 100, IMAGE_WIDTH_EYE, IMAGE_HEIGHT_EYE);
    lcd.drawImage((uint32_t)IMAGE_DATA_MOUTH, 340, 290, IMAGE_WIDTH_MOUTH, IMAGE_HEIGHT_MOUTH);

    while (true) ;

    return 0;
}
