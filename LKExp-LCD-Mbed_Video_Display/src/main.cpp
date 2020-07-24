#include "mbed.h"

#include "main.h"

#define BYTES_PER_PIXEL 4  /* Number of bytes in a pixel */


#define VSYNC           1
#define VBP             1
#define VFP             1
#define VACT            480
#define HSYNC           1
#define HBP             1
#define HFP             1
#define HACT            400

#define LEFT_AREA         1
#define RIGHT_AREA        2

#define PATTERN_SEARCH_BUFFERSIZE 512
/* Private macro -------------------------------------------------------------*/

#define __DSI_MASK_TE()   (GPIOJ->AFR[0] &= (0xFFFFF0FFU))   /* Mask DSI TearingEffect Pin*/
#define __DSI_UNMASK_TE() (GPIOJ->AFR[0] |= ((uint32_t)(GPIO_AF13_DSI) << 8)) /* UnMask DSI TearingEffect Pin*/

#define JPEG_SOI_MARKER (0xFFD8) /* JPEG Start Of Image marker*/
#define JPEG_SOI_MARKER_BYTE0 (JPEG_SOI_MARKER & 0xFF)
#define JPEG_SOI_MARKER_BYTE1 ((JPEG_SOI_MARKER >> 8) & 0xFF)

/* Private variables ---------------------------------------------------------*/
extern uint32_t Previous_FrameSize;
extern LTDC_HandleTypeDef hltdc_discovery;
extern DSI_HandleTypeDef hdsi_discovery;

uint8_t pColLeft[] = { 0x00, 0x00, 0x01, 0x8F }; /*   0 -> 399 */
uint8_t pColRight[] = { 0x01, 0x90, 0x03, 0x1F }; /* 400 -> 799 */
uint8_t pPage[] = { 0x00, 0x00, 0x01, 0xDF }; /*   0 -> 479 */
uint8_t pScanCol[] = { 0x02, 0x15 }; /* Scan @ 533 */

FATFS SDFatFs; /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */
FIL JPEG_File; /* File object */

uint32_t JpegProcessing_End = 0;

JPEG_HandleTypeDef JPEG_Handle;

DMA2D_HandleTypeDef DMA2D_Handle;

static JPEG_ConfTypeDef JPEG_Info;

static __IO int32_t pending_buffer = -1;
static __IO int32_t active_area = 0;
static uint32_t FrameOffset = 0;
static uint8_t PatternSearchBuffer[PATTERN_SEARCH_BUFFERSIZE];
static uint32_t FrameRate;
static uint32_t FrameIndex;

DSI_CmdCfgTypeDef CmdCfg;
DSI_LPCmdTypeDef LPCmd;
DSI_PLLInitTypeDef dsiPllInit;
static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

static char filename[] = "video.avi";

int DSI_IRQ_counter=0;
int DMA2D_IRQ_counter=0;
int HAL_error_status=0;

/* Leka face display from embedded images */
int main(void) {
    DSI_IRQ_counter = 0;
    printf("\n\r--------Programm starting--------\n\r");
    
    uint32_t isfirstFrame, currentFrameRate;
    auto startTime = HAL_GetTick();
    auto endTime = HAL_GetTick();
    char message[16];

    /*##-1- JPEG Initialization ################################################*/   
    /* Init The JPEG Look Up Tables used for YCbCr to RGB conversion   */
    JPEG_InitColorTables();
    /* Init the HAL JPEG driver */
    JPEG_Handle.Instance = JPEG;
    HAL_JPEG_Init(&JPEG_Handle); 


    BSP_LCD_Init();
    printf("LCD_Init done \n\r");
    
    //LCD_LayerInit(0, LCD_FB_START_ADDRESS);
    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(0);
    printf("LCD_LayerInit done \n\r");

    /*HAL_DSI_LongWrite(&hdsi_discovery, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
    HAL_DSI_LongWrite(&hdsi_discovery, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_PASET, pPage);
    // Update pitch : the draw is done on the whole physical X Size
    HAL_LTDC_SetPitch(&hltdc_discovery, OTM8009A_800X480_WIDTH, 0);*/

    pending_buffer = 0;
    active_area = LEFT_AREA;

    BSP_LCD_Clear(LCD_COLOR_CYAN);

    printf("Screen Clear done \n\r");

    //HAL_DSI_LongWrite(&hdsi_discovery, 0, DSI_DCS_LONG_PKT_WRITE, 2, OTM8009A_CMD_WRTESCN, pScanCol);

    // Send Display On DCS Command to display
    HAL_DSI_ShortWrite(
        &(hdsi_discovery),
        0,
        DSI_DCS_SHORT_PKT_WRITE_P1,
        OTM8009A_CMD_DISPON,
        0x00
    );
    //HAL_DSI_Refresh(&hdsi_discovery);

    //printf("DSI IRQ calls : %d \n\r \n\r", DSI_IRQ_counter);
    //##-3- Link the micro SD disk I/O driver ##################################
    if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
        //##-4- Register the file system object to the FatFs module ##############
        if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK) {
            //##-5- Open the JPG file with read access #############################
            if(f_open(&JPEG_File, filename, FA_READ) == FR_OK) {
                printf("File %s openened. File size : %lu \n\r", filename, f_size(&JPEG_File));
                isfirstFrame = 1;
                FrameIndex = 0;
                FrameRate = 0;

                do {
                    //##-6- Find next JPEG Frame offset in the video file #############################
                    FrameOffset =  JPEG_FindFrameOffset(FrameOffset + Previous_FrameSize, &JPEG_File);
                    //printf("Frame offset = %lu \n\r", FrameOffset);
                    if(FrameOffset != 0)
                    {

                        startTime = HAL_GetTick();
                        //printf("Start time %lu \n\r", startTime);
                        f_lseek(&JPEG_File, FrameOffset);
                        //printf("File pointer at position : %lu \n\r",JPEG_File.fptr);
                        //##-7- Start decoding the current JPEG frame with DMA (Not Blocking ) Method ################
                        JPEG_Decode_DMA(&JPEG_Handle, &JPEG_File, JPEG_OUTPUT_DATA_BUFFER);
                        //##-8- Wait till end of JPEG decoding, and perfom Input/Output Processing in BackGround  #
                        do
                        {
                            JPEG_InputHandler(&JPEG_Handle);
                            JpegProcessing_End = JPEG_OutputHandler(&JPEG_Handle);
                        }while(JpegProcessing_End == 0);

                        FrameIndex ++;

                        if(isfirstFrame == 1)
                        {
                            isfirstFrame = 0;
                            //printf("Getting JPEG info \n\r");
                            //##-9- Get JPEG Info  ###############################################
                            HAL_JPEG_GetInfo(&JPEG_Handle, &JPEG_Info);
                            //printf("Initializing DMA2D \n\r");
                            //##-10- Initialize the DMA2D ########################################
                            DMA2D_Init(JPEG_Info.ImageWidth, JPEG_Info.ImageHeight);
                        }
                        //##-11- Copy the Decoded frame to the display frame buffer using the DMA2D #
                        DMA2D_CopyBuffer((uint32_t *)JPEG_OUTPUT_DATA_BUFFER, (uint32_t *)LCD_FRAME_BUFFER, JPEG_Info.ImageWidth, JPEG_Info.ImageHeight);
                        //printf("DMA2D: %d \n\r", DMA2D_IRQ_counter);

                        //##-12- Calc the current decode frame rate #
                        endTime = HAL_GetTick();
                        //printf("End time %lu \n\r", endTime);
                        currentFrameRate = 1000 / (endTime - startTime);
                        sprintf(message ," %lu fps", currentFrameRate);
                        BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)message, CENTER_MODE);
                        FrameRate += currentFrameRate;
//                        printf("-------------\n\r");
                    }

                } while (FrameOffset != 0);

                //##-10- Close the avi file ##########################################
                f_close(&JPEG_File);

            } else printf("Failed to open file %s \n\r", filename);
        } else printf("Mount failed \n\r");
    } else printf("FATFS link failed\n\r");
    printf("Frame offset %lu \n\r", FrameOffset);
    while (true) {
    }

    return 0;
}

/**
 * @brief  Initializes the DSI LCD.
 * The ititialization is done as below:
 *     - DSI PLL ititialization
 *     - DSI ititialization
 *     - LTDC ititialization
 *     - OTM8009A LCD Display IC Driver ititialization
 * @param  None
 * @retval LCD state
 */
uint8_t LCD_Init(void) {
    HAL_StatusTypeDef error_code;
    DSI_PHY_TimerTypeDef  PhyTimings;
    GPIO_InitTypeDef GPIO_Init_Structure;
    /* Toggle Hardware Reset of the DSI LCD using
    * its XRES signal (active low) */
    BSP_LCD_Reset();

    /* Call first MSP Initialize only in case of first initialization
    * This will set IP blocks LTDC, DSI and DMA2D
    * - out of reset
    * - clocked
    * - NVIC IRQ related to IP blocks enabled
    */
    BSP_LCD_MspInit();

    /* LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 417 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 417 MHz / 5 = 83.4 MHz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 83.4 / 2 = 41.7 MHz */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 417;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    error_code = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    printf("Rcc periph pll config : %lu \n\r", (uint32_t)error_code);

    /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
    hdsi_discovery.Instance = DSI;

    error_code = HAL_DSI_DeInit(&(hdsi_discovery));

    dsiPllInit.PLLNDIV  = 100;
    dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
    dsiPllInit.PLLODF   = DSI_PLL_OUT_DIV1;

    hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    hdsi_discovery.Init.TXEscapeCkdiv = 0x4;
    error_code = HAL_DSI_Init(&(hdsi_discovery), &(dsiPllInit));
    printf("DSI Init : %lu \n\r", (uint32_t)error_code);

    /* Configure the DSI for Command mode */
    CmdCfg.VirtualChannelID      = 0;
    CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
    CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
    CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
    CmdCfg.ColorCoding           = DSI_RGB888;
    CmdCfg.CommandSize           = HACT;
    CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
    CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
    CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
    CmdCfg.AutomaticRefresh      = DSI_AR_ENABLE;
    CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
    error_code=  HAL_DSI_ConfigAdaptedCommandMode(&hdsi_discovery, &CmdCfg);
    printf("DSI Config adapted command : %lu \n\r", (uint32_t)error_code);

    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
    error_code = HAL_DSI_ConfigCommand(&hdsi_discovery, &LPCmd);
    printf("DSI Config command : %lu \n\r", (uint32_t)error_code);
    /* Initialize LTDC */
    LTDC_Init();

    /* Start DSI */
    error_code = HAL_DSI_Start(&(hdsi_discovery));
    printf("DSI Start: %lu \n\r", (uint32_t)error_code);

    //BSP_SDRAM_Init();
      /* Configure DSI PHY HS2LP and LP2HS timings */
    PhyTimings.ClockLaneHS2LPTime = 35;
    PhyTimings.ClockLaneLP2HSTime = 35;
    PhyTimings.DataLaneHS2LPTime = 35;
    PhyTimings.DataLaneLP2HSTime = 35;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 10;
    error_code = HAL_DSI_ConfigPhyTimer(&hdsi_discovery, &PhyTimings);
    printf("DSI Config Phy timer : %lu  \n\r\n\r", (uint32_t)error_code);

    /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
    *  depending on configuration set in 'hdsivideo_handle'.
    */
    OTM8009A_Init(OTM8009A_COLMOD_RGB888, LCD_ORIENTATION_LANDSCAPE);

    LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
    LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
    LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
    LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
    LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
    LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
    LPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
    LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
    LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
    LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
    LPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
    error_code = HAL_DSI_ConfigCommand(&hdsi_discovery, &LPCmd);
    printf("DSI Config command : %lu \n\r", (uint32_t)error_code);

    error_code = HAL_DSI_ConfigFlowControl(&hdsi_discovery, DSI_FLOW_CONTROL_BTA);
    printf("DSI Config flow control : %lu \n\r", (uint32_t)error_code);

    // Enable GPIOJ clock 
    __HAL_RCC_GPIOJ_CLK_ENABLE();

    // Configure DSI_TE pin from MB1166 : Tearing effect on separated GPIO from KoD LCD 
    // that is mapped on GPIOJ2 as alternate DSI function (DSI_TE)                      
    // This pin is used only when the LCD and DSI link is configured in command mode    
    GPIO_Init_Structure.Pin       = GPIO_PIN_2;
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_NOPULL;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF13_DSI;
    HAL_GPIO_Init(GPIOJ, &GPIO_Init_Structure);

    // Send Display Off DCS Command to display 
    error_code = HAL_DSI_ShortWrite(
        &(hdsi_discovery),
        0,
        DSI_DCS_SHORT_PKT_WRITE_P1,
        OTM8009A_CMD_DISPOFF,
        0x00
    );

    // Refresh the display 
    error_code = HAL_DSI_Refresh(&hdsi_discovery);
    printf("DSI Refresh : %lu \n\r", (uint32_t)error_code);

    return LCD_OK;
}

void LTDC_Init(void) {
    /* DeInit */
      HAL_LTDC_DeInit(&hltdc_discovery);

      /* LTDC Config */
      /* Timing and polarity */
      hltdc_discovery.Init.HorizontalSync = HSYNC;
      hltdc_discovery.Init.VerticalSync = VSYNC;
      hltdc_discovery.Init.AccumulatedHBP = HSYNC+HBP;
      hltdc_discovery.Init.AccumulatedVBP = VSYNC+VBP;
      hltdc_discovery.Init.AccumulatedActiveH = VSYNC+VBP+VACT;
      hltdc_discovery.Init.AccumulatedActiveW = HSYNC+HBP+HACT;
      hltdc_discovery.Init.TotalHeigh = VSYNC+VBP+VACT+VFP;
      hltdc_discovery.Init.TotalWidth = HSYNC+HBP+HACT+HFP;

      /* background value */
      hltdc_discovery.Init.Backcolor.Blue = 255;
      hltdc_discovery.Init.Backcolor.Green = 0;
      hltdc_discovery.Init.Backcolor.Red = 0;

      /* Polarity */
      hltdc_discovery.Init.HSPolarity = LTDC_HSPOLARITY_AL;
      hltdc_discovery.Init.VSPolarity = LTDC_VSPOLARITY_AL;
      hltdc_discovery.Init.DEPolarity = LTDC_DEPOLARITY_AL;
      hltdc_discovery.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
      hltdc_discovery.Instance = LTDC;
        HAL_StatusTypeDef error;
      error = HAL_LTDC_Init(&hltdc_discovery);
      printf("LTDC init : %d \n\r",error);
}

/**
 * @brief  Initializes the LCD layers.
 * @param  LayerIndex: Layer foreground or background
 * @param  FB_Address: Layer frame buffer
 * @retval None
 */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t Address) {
    LCD_LayerCfgTypeDef Layercfg;

    /* Layer Init */
    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = OTM8009A_800X480_WIDTH/2;
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = OTM8009A_800X480_HEIGHT;
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    Layercfg.FBStartAdress = Address;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 20;
    Layercfg.Backcolor.Green = 200;
    Layercfg.Backcolor.Red = 0;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    Layercfg.ImageWidth = OTM8009A_800X480_WIDTH/2;
    Layercfg.ImageHeight = OTM8009A_800X480_HEIGHT;

    HAL_StatusTypeDef error;
    error = HAL_LTDC_ConfigLayer(&hltdc_discovery, &Layercfg, LayerIndex);
      printf("Layer init : %d \n\r",error);
}


/**
 * @brief  Initialize the DMA2D in memory to memory with PFC.
 * @param  ImageWidth: image width
 * @param  ImageHeight: image Height
 * @retval None
 */
void DMA2D_Init(uint32_t ImageWidth, uint32_t ImageHeight) {
    /* Init DMA2D */
    /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
    DMA2D_Handle.Init.Mode = DMA2D_M2M_PFC; // memory to memory with pixel format convert
    DMA2D_Handle.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    DMA2D_Handle.Init.OutputOffset = OTM8009A_800X480_WIDTH - ImageWidth;
    DMA2D_Handle.Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
    DMA2D_Handle.Init.RedBlueSwap = DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

    /*##-2- DMA2D Callbacks Configuration ######################################*/
    DMA2D_Handle.XferCpltCallback = DMA2D_TransferCompleteCallback;
    DMA2D_Handle.XferErrorCallback = DMA2D_TransferErrorCallback;

    /*##-3- Foreground Configuration ###########################################*/
    DMA2D_Handle.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
    DMA2D_Handle.LayerCfg[1].InputAlpha = 0xFF;
    DMA2D_Handle.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    DMA2D_Handle.LayerCfg[1].InputOffset = 0;
    DMA2D_Handle.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
    DMA2D_Handle.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

    DMA2D_Handle.Instance = DMA2D;

    /*##-4- DMA2D Initialization     ###########################################*/
    HAL_DMA2D_Init(&DMA2D_Handle);
    HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1);
}

/**
 * @brief  Copy the Decoded image to the display Frame buffer.
 * @param  pSrc: Pointer to source buffer
 * @param  pDst: Pointer to destination buffer
 * @param  ImageWidth: image width
 * @param  ImageHeight: image Height
 * @retval None
 */
void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t ImageWidth, uint16_t ImageHeight) {
    uint32_t x = (OTM8009A_800X480_WIDTH - JPEG_Info.ImageWidth) / 2;
    uint32_t y = (OTM8009A_800X480_HEIGHT - JPEG_Info.ImageHeight) / 2;
    uint32_t destination = (uint32_t) pDst + (y * OTM8009A_800X480_WIDTH + x) * 4;
    //printf("DMA copy buffer \n\r");
    /*while (pending_buffer != -1) {
    }*/
    HAL_DMA2D_Start(&DMA2D_Handle, (uint32_t) pSrc, destination, ImageWidth, ImageHeight);
    HAL_DMA2D_PollForTransfer(&DMA2D_Handle, 10);
}

/**
 * @brief  DMA2D Transfer completed callback
 * @param  hdma2d: DMA2D handle.
 * @retval None
 */
void DMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d) {
    // Frame Buffer updated , unmask the DSI TE pin to ask for a DSI refersh
    /*pending_buffer = 1;*/
    // UnMask the TE 
    //__DSI_UNMASK_TE();
    //HAL_DSI_Refresh(&hdsi_discovery);
}

void DMA2D_TransferErrorCallback(DMA2D_HandleTypeDef* hdma2d) {
    OnError_Handler(__FILE__, __LINE__);
}

/**
 * @brief  Find Next JPEG frame offset in the video file.
 * @param  offset: Current offset in the video file.
 * @param  file: Pointer to the video file object.
 * @retval None
 */
uint32_t JPEG_FindFrameOffset(uint32_t offset, FIL *file) {
    uint32_t index = offset, i, readSize = 0;
    do {
        if (f_size(file) <= (index + 1)) {
            /* end of file reached*/
            return 0;
        }
        f_lseek(file, index);
        f_read(file, PatternSearchBuffer, PATTERN_SEARCH_BUFFERSIZE, (UINT*) (&readSize));
        if (readSize != 0) {
            for (i = 0; i < (readSize - 1); i++) {
                if ((PatternSearchBuffer[i] == JPEG_SOI_MARKER_BYTE1) && (PatternSearchBuffer[i + 1] == JPEG_SOI_MARKER_BYTE0)) {
                    return index + i;
                }
            }

            index += (readSize -1);
        }
    } while (readSize != 0);

    return 0;
}

//////// HAL functions ///////////////
/**
 * @brief  Tearing Effect DSI callback.
 * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
 *               the configuration information for the DSI.
 * @retval None
 */
void HAL_DSI_TearingEffectCallback(DSI_HandleTypeDef *hdsi) {
    // Mask the TE 
    //__DSI_MASK_TE();
    //Refresh the right part of the display 
    //HAL_DSI_Refresh(hdsi);
}

/**
 * @brief  End of Refresh DSI callback.
 * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
 *               the configuration information for the DSI.
 * @retval None
 */
void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi) {
    
    /*if (pending_buffer >= 0) {
        if (active_area == LEFT_AREA) {
            // Disable DSI Wrapper
            __HAL_DSI_WRAPPER_DISABLE(hdsi);
            // Update LTDC configuaration
            LTDC_LAYER(&hltdc_discovery, 0)->CFBAR = LCD_FRAME_BUFFER + 400 * 4;
            __HAL_LTDC_RELOAD_CONFIG(&hltdc_discovery);
            // Enable DSI Wrapper
            __HAL_DSI_WRAPPER_ENABLE(hdsi);

            HAL_DSI_LongWrite(hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColRight);
            // Refresh the right part of the display
            HAL_DSI_Refresh(hdsi);

        } else if (active_area == RIGHT_AREA) {
            // Disable DSI Wrapper
            __HAL_DSI_WRAPPER_DISABLE(&hdsi_discovery);
            // Update LTDC configuaration
            LTDC_LAYER(&hltdc_discovery, 0)->CFBAR = LCD_FRAME_BUFFER;
            __HAL_LTDC_RELOAD_CONFIG(&hltdc_discovery);
            // Enable DSI Wrapper
            __HAL_DSI_WRAPPER_ENABLE(&hdsi_discovery);

            HAL_DSI_LongWrite(hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, OTM8009A_CMD_CASET, pColLeft);
            pending_buffer = -1;
        }
    }
    active_area = (active_area == LEFT_AREA) ? RIGHT_AREA : LEFT_AREA;*/
}

////////// END HAL FUNCTIONS ////////////


/**
  * @brief  On Error Handler.
  * @param  None
  * @retval None
  */
void OnError_Handler(const char* file, int line)
{
  printf("Error crash in %s line %d\n\r", file, line);
  while(1) { ; } /* Blocking on error */
}