#include "mbed.h"
#include "include/LekaLCD.h"

/* Bouncing color shift square animation */
int main(void) {
    printf("------Programm starting--------\n\r");

    LekaLCD lcd;

    uint32_t posx = 0;      uint32_t posy = 0;
    uint32_t dirx = 1;      uint32_t diry = 1;
    uint32_t sizex = 100;   uint32_t sizey = 100;

    uint32_t bg_color = 0xffffff00;
    
    // square color
    uint8_t alpha = 0xff;
    uint8_t red = 0xff;
    uint8_t green = 0x00;
    uint8_t blue = 0x00;

    // initialize and select layer 0
    lcd.LTDC_LayerInit(0);
    lcd.setActiveLayer(0);
    // clear layer 0 in yellow
    lcd.clear(bg_color);

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

        HAL_Delay(1); // little delay to let things settle
    }

    return 0;
}
