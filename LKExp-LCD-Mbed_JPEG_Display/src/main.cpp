#include "mbed.h"
#include "include/LekaLCD.h"
#include "include/img/leka_eye_alpha.h"
#include "include/img/eye_yel.h"
#include "include/img/leka_mouth_alpha.h"

#include "BSP_DISCO_F769NI/Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.h"
#include "BSP_DISCO_F769NI/Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.h"

#include "FatFs/ff_gen_drv.h"
#include "FatFs/ff.h"
#include "FatFs/drivers/sd_diskio.h"
#include "include/jpeg_utils.h"
#include "decode_dma.h"

#define LCD_FRAME_BUFFER         0xC0000000
#define JPEG_OUTPUT_DATA_BUFFER  0xC0200000 


FATFS SDFatFs;  /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */
FIL JPEG_File;  /* File object */

static void DMA2D_CopyBuffer(DMA2D_HandleTypeDef* DMA2D_Handle, uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);

/* Leka face display from embedded images */
int main(void) {
    printf("--------Programm starting--------\n\r");

    LekaLCD lcd;

    /*##-1- JPEG Initialization ################################################*/   
    /* Init The JPEG Look Up Tables used for YCbCr to RGB conversion   */
    JPEG_InitColorTables();
    JPEG_HandleTypeDef JPEG_Handle;
    JPEG_ConfTypeDef JPEG_Info;
    uint32_t JpegProcessing_End = 0;
    
    /* Init the HAL JPEG driver */
    JPEG_Handle.Instance = JPEG;
    HAL_JPEG_Init(&JPEG_Handle); 
    
    //##-3- Link the micro SD disk I/O driver ##################################
    if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
        printf("FATS Driver linked\n\r");
        //##-4- Register the file system object to the FatFs module ##############
        if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK) {
            printf("Mouted SDPATH : %s \n\r", SDPath);
            //##-5- Open the JPG file with read access #############################
            if(f_open(&JPEG_File, "image.jpg", FA_READ) == FR_OK) {
                printf("File image.jp openened \n\r");
                //##-6- JPEG decoding with DMA (Not Blocking ) Method ################
                //JPEG_Decode_DMA(&JPEG_Handle, &JPEG_File, JPEG_OUTPUT_DATA_BUFFER); // <- Error in this function, on HAL_JPEG_Decode_DMA call
                // TODO : investigate error 

                //##-7- Wait till end of JPEG decoding and perfom Input/Output Processing in BackGround  #
                /*do {
                    JPEG_InputHandler(&JPEG_Handle);
                    JpegProcessing_End = JPEG_OutputHandler(&JPEG_Handle);
          
                }   
                while(JpegProcessing_End == 0);
        
                //##-8- Get JPEG Info  ###############################################
                HAL_JPEG_GetInfo(&JPEG_Handle, &JPEG_Info);       
        
                DMA2D_CopyBuffer(&lcd._handle_dma2d, (uint32_t *)JPEG_OUTPUT_DATA_BUFFER, (uint32_t *)LCD_FRAME_BUFFER, 0 , 0, JPEG_Info.ImageWidth, JPEG_Info.ImageHeight);
                */
                //##-10- Close the JPG file ##########################################
                f_close(&JPEG_File);
            } else printf("Failed to open file \n\r");
        } else printf("Mount failed \n\r");
    }else printf("FATFS link failed\n\r");

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
        //lcd.fillRect(posx, posy, sizex, sizey, (alpha<<24) | (red<<16) | (green<<8) | (blue) );

        // update colors
        if(green == 0) {red--; blue++;}
        if(red == 0) {green++; blue--;}
        if(blue == 0) {red++; green--;}
        HAL_Delay(2);
    }

    return 0;
}

static void DMA2D_CopyBuffer(DMA2D_HandleTypeDef* DMA2D_Handle, uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{   
  
  uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
  uint32_t source      = (uint32_t)pSrc;
  
  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/ 
  DMA2D_Handle->Init.Mode          = DMA2D_M2M_PFC;
  DMA2D_Handle->Init.ColorMode     = DMA2D_OUTPUT_ARGB8888;
  DMA2D_Handle->Init.OutputOffset  = 800 - xsize;
  DMA2D_Handle->Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/  
  DMA2D_Handle->Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */    
  
  /*##-2- DMA2D Callbacks Configuration ######################################*/
  DMA2D_Handle->XferCpltCallback  = NULL;
  
  /*##-3- Foreground Configuration ###########################################*/
  DMA2D_Handle->LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  DMA2D_Handle->LayerCfg[1].InputAlpha = 0xFF;
 
  DMA2D_Handle->LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;

  DMA2D_Handle->LayerCfg[1].InputOffset = 0;
  DMA2D_Handle->LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  DMA2D_Handle->LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */  

  DMA2D_Handle->Instance = DMA2D; 
   
  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(DMA2D_Handle) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(DMA2D_Handle, 1) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(DMA2D_Handle, source, destination, xsize, ysize) == HAL_OK)
      {
        /* Polling For DMA transfer */  
        HAL_DMA2D_PollForTransfer(DMA2D_Handle, 100);
      }
    }
  }   
}