#include "include/LekaLCD.h"
#include "include/drivers/otm8009a.h"



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

    ///////////// DSI Initialization /////////////////////////
    _handle_dsi.Instance = DSI;
    HAL_DSI_DeInit(&_handle_dsi);
    _handle_dsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;   // number of lanes -> 2
    // TXEscapeCKdiv = lane_byte_clock_kHz / 15620 = 4
    _handle_dsi.Init.TXEscapeCkdiv = lane_byte_clock_kHz / 15620;
    HAL_DSI_Init(&_handle_dsi, &dsi_pll_init);

    // video synchronisation parameters, all values in units of line (= in pixels ?)
    // these values depend on orientation, I picked landscape values
    uint32_t VSA = OTM8009A_480X800_VSYNC;              // Vertical start active time 
    uint32_t VBP = OTM8009A_480X800_VBP;                // Vertical back porch time
    uint32_t VFP = OTM8009A_480X800_VFP;                // Vertical front porch time
    uint32_t VACT = _screen_height;                     // Vertical Active time
    uint32_t HSA = OTM8009A_480X800_HSYNC;              // Idem for horizontal
    uint32_t HBP = OTM8009A_480X800_HBP;
    uint32_t HFP = OTM8009A_480X800_HFP;
    uint32_t HACT = _screen_width;

    _handle_dsivideo.VirtualChannelID = 0;      // LCD_OTM8009A_ID = 0
    _handle_dsivideo.ColorCoding = DSI_RGB888;
    _handle_dsivideo.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    _handle_dsivideo.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    _handle_dsivideo.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    _handle_dsivideo.Mode = DSI_VID_MODE_BURST;
    _handle_dsivideo.NullPacketSize = 0xFFF;
    _handle_dsivideo.NumberOfChunks = 0;
    _handle_dsivideo.PacketSize = HACT;

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

    /////////////////////////// End DSI Initialization ///////////////////////////

    /////////////////////////// LTDC Initialization ///////////////////////////

    // timing configuration (LCD-TFT documentation section 3.2.2)
    _handle_ltdc.Init.HorizontalSync = HSA - 1;
    _handle_ltdc.Init.AccumulatedHBP = HSA + HBP - 1;
    _handle_ltdc.Init.AccumulatedActiveW = _screen_width + HSA + HBP - 1;
    _handle_ltdc.Init.TotalWidth = _screen_width + HSA + HBP + HFP - 1;

    _handle_ltdc.LayerCfg->ImageWidth = _screen_width;
    _handle_ltdc.LayerCfg->ImageHeight = _screen_height;

    // LCD clock configuration
    static RCC_PeriphCLKInitTypeDef periph_clk_init;
    periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init.PLLSAI.PLLSAIN = 384;
    periph_clk_init.PLLSAI.PLLSAIR = 7;
    periph_clk_init.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init);

    // background value
    _handle_ltdc.Init.Backcolor.Blue = 0;
    _handle_ltdc.Init.Backcolor.Green = 50;
    _handle_ltdc.Init.Backcolor.Red = 0;
    _handle_ltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    _handle_ltdc.Instance = LTDC;

    // init ltdc from dsivideo config 
    // (basically copy horizontal and vertical synchronization values from dsivideo to ltdc)
    HAL_LTDC_StructInitFromVideoConfig(&_handle_ltdc, &_handle_dsivideo);

    HAL_LTDC_Init(&_handle_ltdc);

    HAL_DSI_Start(&_handle_dsi); 

    // HERE : need to add external SDRAM initialization
    
    // this command takes too much time why ?
    OTM8009A_Init(OTM8009A_FORMAT_RGB888, OTM8009A_ORIENTATION_LANDSCAPE);

    LTDC_LayerInit(0);
    //LTDC_LayerInit(1);
}


uint32_t LekaLCD::getScreenWidth() {
    return _screen_width;
}

uint32_t LekaLCD::getScreenHeight() {
    return _screen_height;
}

void LekaLCD::setActiveLayer(uint16_t layer_index) {
    _active_layer = layer_index;
}

void LekaLCD::clear(uint32_t color) {
    fillBuffer(_active_layer, (uint32_t*)(_frame_buffer_start_address), _screen_width, _screen_height, 0, color);
}

void LekaLCD::drawPixel(uint32_t x, uint32_t y, uint32_t color) {
    *(__IO uint32_t*) (_frame_buffer_start_address + (4*(y*_screen_width + x))) = color;
}


void LekaLCD::fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    uint32_t dest_address = (_handle_ltdc.LayerCfg[_active_layer].FBStartAdress) + 4*(_screen_width*y + x);
    uint32_t offset = _screen_width - width;

    fillBuffer(_active_layer, (uint32_t*)dest_address, width, height, offset, color);
}

void LekaLCD::fillBuffer(uint32_t layer_index, void* dest_addr, uint32_t width, uint32_t height, uint32_t offset, uint32_t color) {
    _handle_dma2d.Init.Mode = DMA2D_R2M;
    _handle_dma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    _handle_dma2d.Init.OutputOffset = offset;
    _handle_dma2d.Instance = DMA2D;
    
    if(HAL_DMA2D_Init(&_handle_dma2d) == HAL_OK ) {
        if(HAL_DMA2D_ConfigLayer(&_handle_dma2d, layer_index) == HAL_OK){
            if(HAL_DMA2D_Start(&_handle_dma2d, color, (uint32_t)dest_addr, width, height) == HAL_OK){
                HAL_DMA2D_PollForTransfer(&_handle_dma2d, 10);
            }
        }
    }
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

// Layer init (copied from BSP)
void LekaLCD::LTDC_LayerInit(uint16_t layer_index) {
	LTDC_LayerCfgTypeDef  Layercfg;

    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = 800;
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = 480; 
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    Layercfg.FBStartAdress = _frame_buffer_start_address;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 0;
    Layercfg.Backcolor.Green = 0;
    Layercfg.Backcolor.Red = 0;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    Layercfg.ImageWidth = 800;
    Layercfg.ImageHeight = 480;
  
    HAL_LTDC_ConfigLayer(&_handle_ltdc, &Layercfg, layer_index);
}

// DSI write commands
void LekaLCD::DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams) {
    if(NbrParams <= 1)
    {
        HAL_DSI_ShortWrite(&_handle_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]); 
    }
    else
    {
        HAL_DSI_LongWrite(&_handle_dsi,  0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams); 
    } 
}

// OTM8009A driver initialization
uint8_t LekaLCD::OTM8009A_Init(uint32_t ColorCoding, uint32_t orientation) {
  /* Enable CMD2 to access vendor specific commands                               */
  /* Enter in command 2 mode and set EXTC to enable address shift function (0x00) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd( 3, (uint8_t *)lcdRegData1);

  /* Enter ORISE Command 2 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2); /* Shift address to 0x80 */
    DSI_IO_WriteCmd( 2, (uint8_t *)lcdRegData2);

  /////////////////////////////////////////////////////////////////////
  /* SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          */
  /* Set SD_PT                                                       */
  /* -> Source output level during porch and non-display area to GND */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData3);
    HAL_Delay(10);
  /* Not documented */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData4);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData5);
    HAL_Delay(10);
  /////////////////////////////////////////////////////////////////////

  /* PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 */
  /* Set gvdd_en_test                                     */
  /* -> enable GVDD test mode !!!                         */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData6);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData7);

  /* PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      */
  /* Set pump 4 vgh voltage                                    */
  /* -> from 15.0v down to 13.0v                               */
  /* Set pump 5 vgh voltage                                    */
  /* -> from -12.0v downto -9.0v                               */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData8);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);

  /* P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 */
  /* -> Column inversion                                */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData10);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData11);

  /* VCOMDC - 0xD900h - 1st parameter - Default 0x39h */
  /* VCOM Voltage settings                            */
  /* -> from -1.0000v downto -1.2625v                 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData12);

  /* Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData14);

  /* Video mode internal */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData16);

  /* PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 */
  /* Set pump 4&5 x6                                     */
  /* -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData17);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData18);

  /* PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h */
  /* Change pump4 clock ratio                              */
  /* -> from 1 line to 1/2 line                            */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData19);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);

  /* GVDD/NGVDD settings */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd( 2, (uint8_t *)lcdRegData5);

  /* PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h */
  /* Rewrite the default value !                           */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData20);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData21);

  /* Panel display timing Setting 3 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData22);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData23);

  /* Power control 1 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData24);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData25);

  /* Source driver precharge */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData26);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData27);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData28);
    DSI_IO_WriteCmd( 2, (uint8_t *)lcdRegData6);

  /* GOAVST */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd( 6, (uint8_t *)lcdRegData7);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd( 14, (uint8_t *)lcdRegData8);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd( 14, (uint8_t *)lcdRegData9);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData10);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData46);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData11);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData12);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData13);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData14);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData15);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData16);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData34);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData17);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData35);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData18);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData19);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData20);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData21);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd( 10, (uint8_t *)lcdRegData22);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData23);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd( 15, (uint8_t *)lcdRegData24);

  /////////////////////////////////////////////////////////////////////////////
  /* PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 */
  /* Pump 1 min and max DM                                */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData47);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData48);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData49);
  /////////////////////////////////////////////////////////////////////////////

  /* CABC LEDPWM frequency adjusted to 19,5kHz */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData50);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData51);
  
  /* Exit CMD2 mode */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd( 3, (uint8_t *)lcdRegData25);

  /*************************************************************************** */
  /* Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   */
  /*************************************************************************** */

  /* NOP - goes back to DCS std command ? */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
          
  /* Gamma correction 2.2+ table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd( 16, (uint8_t *)lcdRegData3);
  
  /* Gamma correction 2.2- table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd( 16, (uint8_t *)lcdRegData4);
          
  /* Send Sleep Out command to display : no parameter */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);
  
  /* Wait for sleep out exit */
    HAL_Delay(120);

    switch(ColorCoding)
    {
    case OTM8009A_FORMAT_RBG565 :
    /* Set Pixel color format to RGB565 */
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData37);
        break;
    case OTM8009A_FORMAT_RGB888 :
    /* Set Pixel color format to RGB888 */
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData38);
        break;
    default :
        break;
    }

  /* Send command to configure display in landscape orientation mode. By default
      the orientation mode is portrait  */
    if(orientation == OTM8009A_ORIENTATION_LANDSCAPE)
    {
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData39);
        DSI_IO_WriteCmd( 4, (uint8_t *)lcdRegData27);
        DSI_IO_WriteCmd( 4, (uint8_t *)lcdRegData28);
    }

  /** CABC : Content Adaptive Backlight Control section start >> */
  /* Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
  DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData40);

  /* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData41);

  /* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData42);

  /* defaut is 0 (lowest Brightness), 0xFF is highest Brightness */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData43);

  /** CABC : Content Adaptive Backlight Control section end << */

  /* Send Command Display On */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);

  /* NOP command */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);

  /* Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
  /* DSI host from LTDC incoming pixels in video mode */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData45);

    return 0;
}
