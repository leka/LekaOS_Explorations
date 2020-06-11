

#ifndef __LEKALCD_H__
#define __LEKALCD_H__

#include "mbed.h"
#define LCD_DSI_ID 0x11

class LekaLCD {

public:
    // constructor does all the initialization stuff, 
    // might move it to an init() method
    LekaLCD();

    void reset();

    uint32_t getScreenWidth();
    uint32_t getScreenHeight();

private:
    DMA2D_HandleTypeDef _handle_dma2d;
    LTDC_HandleTypeDef _handle_ltdc;
    DSI_HandleTypeDef _handle_dsi;
    DSI_VidCfgTypeDef _handle_dsivideo;

    // using landscape orientation by default
    const uint32_t _screen_width = 800;
    const uint32_t _screen_height = 480;

    void MspInit();
};

// experimental DMA2D
/* 
DMA2D_HandleTypeDef hdma2d;

void DMA2D_FillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
        hdma2d.Instance = DMA2D;
        hdma2d.Init.Mode = DMA2D_R2M;
        hdma2d.Init.ColorMode = DMA2D_ARGB8888;
        hdma2d.Init.OutputOffset = 0;
        HAL_DMA2D_Init(&hdma2d);
        HAL_DMA2D_Start(&hdma2d, color, (uint32_t)0xC0000000, width, height);
        HAL_DMA2D_PollForTransfer(&hdma2d, 10);

}
*/
#endif