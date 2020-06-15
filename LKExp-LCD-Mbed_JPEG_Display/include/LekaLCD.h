

#ifndef __LEKALCD_H__
#define __LEKALCD_H__

#include "mbed.h"

class LekaLCD {

public:
    /**
     * @brief Construct a new Leka LCDD object and 
     * does all the initialization needed
     */
    LekaLCD();

    uint32_t getScreenWidth();
    uint32_t getScreenHeight();
    void setActiveLayer(uint16_t layer_index);

    /**
     * @brief Clears the screen with a color
     * 
     * @param color     : Color
     */
    void clear(uint32_t color);

    /**
     * @brief Draws a pixel in a color
     * 
     * @param x     : x position
     * @param y     : y position
     * @param color : color of the pixel
     */
    void drawPixel(uint32_t x, uint32_t y, uint32_t color);
    
    /**
     * @brief Fills a rectangle on the active layer with a color
     * 
     * @param x         : x position
     * @param y         : y position 
     * @param width     : width
     * @param height    : height
     * @param color     : color
     */
    void fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);

private:
    DMA2D_HandleTypeDef _handle_dma2d;
    LTDC_HandleTypeDef _handle_ltdc;
    DSI_HandleTypeDef _handle_dsi;
    DSI_VidCfgTypeDef _handle_dsivideo;

    // using landscape orientation by default
    const uint32_t _screen_width = 800;
    const uint32_t _screen_height = 480;

    const uint32_t _frame_buffer_start_address = 0xC0000000;

    // active layer can be either 0 or 1 (LTC supports 2 layers)
    uint16_t _active_layer = 0;
    
    /**
     * @brief Fills a rectangle in the frame buffer
     * 
     * @param layer_index : Layer index where to draw (0 or 1)
     * @param dest_addr   : Frame buffer start address
     * @param width       : Width of the rectangle
     * @param height      : Height of the rectangle
     * @param offset      : offset = screen_width - rectangle_width
     * @param color       : Color
     */
    void fillBuffer(uint32_t layer_index, void* dest_addr, uint32_t width, uint32_t height, uint32_t offset, uint32_t color);
    
    // internal init functions
    void reset();
    void MspInit();
    void LTDC_LayerInit(uint16_t layer_index);
    void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams);
    uint8_t OTM8009A_Init(uint32_t ColorCoding, uint32_t orientation);
};

#endif
