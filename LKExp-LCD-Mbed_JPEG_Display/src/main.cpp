#include "mbed.h"
#include "include/LekaLCD.h"
#include "include/img/leka_eye_alpha.h"
#include "include/img/eye_yel.h"
#include "include/img/leka_mouth_alpha.h"

/* Leka face display from embedded images */
int main(void) {
    printf("--------Programm starting--------\n\r");

    LekaLCD lcd;

    uint32_t posx = 0;     uint32_t posy = 0;
    int8_t dirx = 1;       int8_t diry = 1;
    uint8_t sizex = 100;   uint8_t sizey = 100;

    // square color
    uint8_t alpha = 0xff;
    uint8_t red = 0xff;
    uint8_t green = 0x00;
    uint8_t blue = 0x00;

    uint32_t bg_color = 0xffffffff;
    
    // initialize and select layer 0
    lcd.LTDC_LayerInit(0);
    //lcd.LTDC_LayerInit(1);
    lcd.setActiveLayer(0);
    // clear layer 0 in white
    lcd.clear(bg_color);

    lcd.drawImage((uint32_t)IMAGE_DATA_EYE,  450, 100, IMAGE_WIDTH_EYE, IMAGE_HEIGHT_EYE);
    lcd.drawImage((uint32_t)IMAGE_DATA_YEL,  500, 100, IMAGE_WIDTH_YEL, IMAGE_HEIGHT_EYE);
    lcd.drawImage((uint32_t)IMAGE_DATA_MOUTH, 340, 290, IMAGE_WIDTH_MOUTH, IMAGE_HEIGHT_MOUTH);

    while (true) {
         // update position
        posx = (posx + dirx);
        posy = (posy + diry);

        // chek for screen limits
        if(posx >= 800-sizex || posx <= 0) {dirx *= -1;}
        if(posy >= 480-sizey || posy <= 0) {diry *= -1;}

        // draw the square
        lcd.fillRect(posx, posy, sizex, sizey, (alpha<<24) | (red<<16) | (green<<8) | (blue) );

        // update colors
        if(green == 0) {red--; blue++;}
        if(red == 0) {green++; blue--;}
        if(blue == 0) {red++; green--;}
        HAL_Delay(2);
    }

    return 0;
}
