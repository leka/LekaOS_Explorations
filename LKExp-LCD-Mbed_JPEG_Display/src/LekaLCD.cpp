#include "include/LekaLCD.h"


LekaLCD::LekaLCD() {
    reset();
    MspInit();

    // configuring clock for DSI (following BSP)
    DSI_PLLInitTypeDef dsi_pll_init;
    dsi_pll_init.PLLNDIV = 100;             // clock loop division factor
    dsi_pll_init.PLLIDF = DSI_PLL_IN_DIV5;   // clock input division factor
    dsi_pll_init.PLLODF = DSI_PLL_OUT_DIV1;  // clock output division factor
    uint32_t lane_byte_clock_kHz = 62500;            // 500 MHz / 8 = 62.5 MHz = 62500 KHz
    uint32_t lcd_clock = 27429;  // 27429 kHz

    /**************************** DSI Initialization *************************************/
    _handle_dsi.Instance = DSI;
    HAL_DSI_DeInit(&_handle_dsi);
    _handle_dsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;   // number of lanes -> 2
    // TXEscapeCKdiv = lane_byte_clock_kHz / 15620 = 4
    _handle_dsi.Init.TXEscapeCkdiv = lane_byte_clock_kHz / 15620;
    HAL_DSI_Init(&_handle_dsi, &dsi_pll_init);

    // video synchronisation parameters, all values in units of line (= in pixels ?)
    // these values depend on orientation, I picked landscape values
    uint32_t VSA = 1;               // Vertical start active time 
    uint32_t VBP = 15;               // Vertical back porch time
    uint32_t VFP = 16;               // Vertical front porch time
    uint32_t VACT = _screen_height; // Vertical Active time
    uint32_t HSA = 2;               // Idem for horizontal
    uint32_t HBP = 34;
    uint32_t HFP = 34;
    uint32_t HACT = _screen_width;

    _handle_dsivideo.VirtualChannelID = 0;
    _handle_dsivideo.ColorCoding = DSI_RGB888;
    _handle_dsivideo.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    _handle_dsivideo.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    _handle_dsivideo.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    _handle_dsivideo.Mode = DSI_VID_MODE_BURST;
    _handle_dsivideo.NullPacketSize = 0xFFF;
    _handle_dsivideo.NumberOfChunks = 0;
    _handle_dsivideo.PacketSize = HACT;

    // these formulas can be found in the LCD-TFT documentation section 3.2.2
    _handle_dsivideo.HorizontalSyncActive = (HSA * lane_byte_clock_kHz) / lcd_clock;
    _handle_dsivideo.HorizontalBackPorch = (HBP * lane_byte_clock_kHz) / lcd_clock;
    _handle_dsivideo.HorizontalLine = ((HACT + HSA + HBP + HFP) * lane_byte_clock_kHz) / lcd_clock;
    _handle_dsivideo.VerticalSyncActive = VSA;
    _handle_dsivideo.VerticalBackPorch = VBP;
    _handle_dsivideo.VerticalFrontPorch = VFP;
    _handle_dsivideo.VerticalActive = VACT;

    // enable sending commands in low power mode
    // maybe we want to disable it ?
    _handle_dsivideo.LPCommandEnable = DSI_LP_COMMAND_ENABLE;

    _handle_dsivideo.LPLargestPacketSize = 16;      // low power largest packet
    _handle_dsivideo.LPVACTLargestPacketSize = 0;   // low power largest packet during VACT period

    _handle_dsivideo.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;  // Allow sending LP commands during HFP period 
    _handle_dsivideo.LPHorizontalBackPorchEnable  = DSI_LP_HBP_ENABLE;  // Allow sending LP commands during HBP period
    _handle_dsivideo.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;       // Allow sending LP commands during VACT period
    _handle_dsivideo.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;    // Allow sending LP commands during VFP period
    _handle_dsivideo.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;     // Allow sending LP commands during VBP period
    _handle_dsivideo.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;  // Allow sending LP commands during VSync = VSA period

    // configure DSI Video Mode timings with all the settings we defined
    HAL_DSI_ConfigVideoMode(&_handle_dsi, &_handle_dsivideo);

    /******************************* End DSI Initialization *********************************/

    // TODO : LTDC initialization
    // TODO : OTM8009A (name of the LCD screen) driver initialization (need to copy otm8009a.h into my working directory)
}

// copied from BSP
void LekaLCD::reset() {
    GPIO_InitTypeDef  gpio_init_structure;

    __HAL_RCC_GPIOJ_CLK_ENABLE();

    /* Configure the GPIO on PJ15 */
    gpio_init_structure.Pin   = GPIO_PIN_15;
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

    /* Activate XRES active low */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_RESET);

    HAL_Delay(20); /* wait 20 ms */

    /* Desactivate XRES */
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_SET);
    
    /* Wait for 10ms after releasing XRES before sending commands */
    HAL_Delay(10);
}

uint32_t LekaLCD::getScreenWidth() {
    return _screen_width;
}

uint32_t LekaLCD::getScreenHeight() {
    return _screen_height;
}

// copied from BSP
void LekaLCD::MspInit() {
    /** @brief Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /** @brief Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    /** @brief Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /** @brief Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    /** @brief Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /** @brief Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();

    /** @brief NVIC configuration for LTDC interrupt that is now enabled */
    HAL_NVIC_SetPriority(LTDC_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);

    /** @brief NVIC configuration for DMA2D interrupt that is now enabled */
    HAL_NVIC_SetPriority(DMA2D_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);

    /** @brief NVIC configuration for DSI interrupt that is now enabled */
    HAL_NVIC_SetPriority(DSI_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DSI_IRQn);
}