/**
 ******************************************************************************
 * @file    JPEG/JPEG_DecodingUsingFs_DMA/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use the HW JPEG to Decode a JPEG file with DMA method.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal_uart.h"
#include <stdio.h>
/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @addtogroup JPEG_DecodingUsingFs_DMA
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#define PATTERN_SEARCH_BUFFERSIZE 512
/* Private macro -------------------------------------------------------------*/
#define JPEG_SOI_MARKER (0xFFD8) /* JPEG Start Of Image marker*/
#define JPEG_SOI_MARKER_BYTE0 (JPEG_SOI_MARKER & 0xFF)
#define JPEG_SOI_MARKER_BYTE1 ((JPEG_SOI_MARKER >> 8) & 0xFF)

/* Private variables ---------------------------------------------------------*/
extern uint32_t Previous_FrameSize;
extern LTDC_HandleTypeDef hltdc_discovery;
extern DSI_HandleTypeDef hdsi_discovery;

FATFS SDFatFs; /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */
FIL JPEG_File; /* File object */

uint32_t JpegProcessing_End = 0;

JPEG_HandleTypeDef JPEG_Handle;

DMA2D_HandleTypeDef DMA2D_Handle;

static JPEG_ConfTypeDef JPEG_Info;


static uint32_t FrameOffset = 0;
static uint8_t PatternSearchBuffer[PATTERN_SEARCH_BUFFERSIZE];
static uint32_t FrameRate;
static uint32_t FrameIndex;

DSI_CmdCfgTypeDef CmdCfg;
DSI_LPCmdTypeDef LPCmd;
DSI_PLLInitTypeDef dsiPllInit;

UART_HandleTypeDef UartHandle;

static char filename[] = "video.avi";

/* Private function prototypes -----------------------------------------------*/
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

static void LCD_FileErrorDisplay(void);
static void LCD_DowncountDisplay(void);


static uint32_t JPEG_FindFrameOffset(uint32_t offset, FIL *file);

static void DMA2D_Init(uint32_t ImageWidth, uint32_t ImageHeight);
static void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t ImageWidth, uint16_t ImageHeight);


static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
	/*uint32_t xPos = 0, yPos = 0;
	uint32_t width_offset = 0;
	uint8_t lcd_status = LCD_OK;*/

	uint32_t isfirstFrame, startTime, endTime, currentFrameRate;
	char message[16];

	/* Enable the CPU Cache */
	CPU_CACHE_Enable();

	/* STM32F7xx HAL library initialization:
	 - Configure the Flash ART accelerator on ITCM interface
	 - Systick timer is configured by default as source of time base, but user
	 can eventually implement his proper time base source (a general purpose
	 timer for example or other time source), keeping in mind that Time base
	 duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	 handled in milliseconds basis.
	 - Set NVIC Group Priority to 4
	 - Low Level Initialization
	 */
	HAL_Init();

	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Initialize the LED1 (Red LED , set to On when error) */
	BSP_LED_Init(LED2);

	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
	 - Word Length = 8 Bits (7 data bit + 1 parity bit) :
	 BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
	 - Stop Bit    = One Stop bit
	 - Parity      = ODD parity
	 - BaudRate    = 9600 baud
	 - Hardware flow control disabled (RTS and CTS signals) */
	UartHandle.Instance = USARTx;

	UartHandle.Init.BaudRate = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_ODD;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode = UART_MODE_TX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
		/* Initialization Error */
		OnError_Handler();
	}


	/*##-1- JPEG Initialization ################################################*/
	/* Init The JPEG Look Up Tables used for YCbCr to RGB conversion   *//* Init The JPEG Color Look Up Tables used for YCbCr to RGB conversion   */
	JPEG_InitColorTables();

	/* Init the HAL JPEG driver */
	JPEG_Handle.Instance = JPEG;
	HAL_JPEG_Init(&JPEG_Handle);

	/*##-2- LCD Configuration ##################################################*/
	/* Initialize the LCD   */
	BSP_LCD_Init();

	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);

	// Clear LCD
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	// Send Display On DCS Command to display
	HAL_DSI_ShortWrite(
		&(hdsi_discovery),
		0,
		DSI_DCS_SHORT_PKT_WRITE_P1,
		OTM8009A_CMD_DISPON,
		0x00
	);

	//##-3- Link the micro SD disk I/O driver ##################################
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		//##-4- Register the file system object to the FatFs module ##############
		if (f_mount(&SDFatFs, (TCHAR const*) SDPath, 0) == FR_OK) {
			//##-5- Open the JPG file with read access #############################
			if (f_open(&JPEG_File, filename, FA_READ) == FR_OK) {
				isfirstFrame = 1;
				FrameIndex = 0;
				FrameRate = 0;
				printf("video avi opened \n\r");
				LCD_DowncountDisplay();
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
						//printf("Decoding DMA ... \n\r");

						//##-7- Start decoding the current JPEG frame with DMA (Not Blocking ) Method ################
						JPEG_Decode_DMA(&JPEG_Handle, &JPEG_File, JPEG_OUTPUT_DATA_BUFFER);
						//printf("Decoding DMA ended \n\r");

						//##-8- Wait till end of JPEG decoding, and perfom Input/Output Processing in BackGround  #
						do
						{
							JPEG_InputHandler(&JPEG_Handle);
							JpegProcessing_End = JPEG_OutputHandler(&JPEG_Handle);
						}while(JpegProcessing_End == 0);

						FrameIndex ++;
						//printf("Frame index : %d\n\r", FrameIndex);

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

						//##-12- Calc the current decode frame rate #
						endTime = HAL_GetTick();
						//printf("End time %lu \n\r", endTime);
						currentFrameRate = 1000 / (endTime - startTime);
						sprintf(message ," %lu fps", currentFrameRate);
						BSP_LCD_DisplayStringAtLine(29, (uint8_t *)message);
						FrameRate += currentFrameRate;
					}
					// looping video
					else if (1){
						FrameOffset = 1;
						f_lseek(&JPEG_File, 0);
					}

				} while (FrameOffset != 0);

				//##-10- Close the avi file ##########################################
				f_close(&JPEG_File);

			}
			else // Can't Open JPG file
			{
				//printf("video avi not opened \n\r .");
				LCD_FileErrorDisplay();
			}
		}
	}

	// Infinite loop
	while (1) {

	}
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


static void LCD_DowncountDisplay(void) {
	uint32_t i = 5;
	char message[32];
	//printf("LCD_ downcount \n\r");

	BSP_LCD_Clear(LCD_COLOR_BLUE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	//HAL_DSI_Refresh(&hdsi_discovery);
	HAL_Delay(20);

	while (i > 0) {
		sprintf(message, "%lu", i);
		BSP_LCD_DisplayStringAt(0, 232, (uint8_t*) message, CENTER_MODE);

		HAL_Delay(500);
		i--;
	}

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
}

/**
 * @brief  Display File access error message.
 * @param  None
 * @retval None
 */
static void LCD_FileErrorDisplay(void) {
	char message[100];

	BSP_LCD_Clear(LCD_COLOR_RED);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_SetFont(&Font16);

	HAL_DSI_Refresh(&hdsi_discovery);
	HAL_Delay(20);

	sprintf(message, "Unable to open file %s", filename);
	BSP_LCD_DisplayStringAtLine(3, (uint8_t*) message);

	HAL_DSI_Refresh(&hdsi_discovery);
	HAL_Delay(20);

	sprintf(message, "Please check that a file named %s is stored on the uSD", filename);
	BSP_LCD_DisplayStringAtLine(5, (uint8_t*) message);

	HAL_DSI_Refresh(&hdsi_discovery);
	HAL_Delay(20);
}


/**
 * @brief  Initialize the DMA2D in memory to memory with PFC.
 * @param  ImageWidth: image width
 * @param  ImageHeight: image Height
 * @retval None
 */
static void DMA2D_Init(uint32_t ImageWidth, uint32_t ImageHeight) {
	/* Init DMA2D */
	/*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
	DMA2D_Handle.Init.Mode = DMA2D_M2M_PFC; // memory to memory with pixel format convert
	DMA2D_Handle.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
	DMA2D_Handle.Init.OutputOffset = OTM8009A_800X480_WIDTH - ImageWidth;
	DMA2D_Handle.Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
	DMA2D_Handle.Init.RedBlueSwap = DMA2D_RB_REGULAR; /* No Output Red & Blue swap */


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
static void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t ImageWidth, uint16_t ImageHeight) {
	uint32_t x = (OTM8009A_800X480_WIDTH - JPEG_Info.ImageWidth) / 2;
	uint32_t y = (OTM8009A_800X480_HEIGHT - JPEG_Info.ImageHeight) / 2;
	uint32_t destination = (uint32_t) pDst + (y * OTM8009A_800X480_WIDTH + x) * 4;

	HAL_DMA2D_Start_IT(&DMA2D_Handle, (uint32_t) pSrc, destination, ImageWidth, ImageHeight);
}


/**
 * @brief  Find Next JPEG frame offset in the video file.
 * @param  offset: Current offset in the video file.
 * @param  file: Pointer to the video file object.
 * @retval None
 */
static uint32_t JPEG_FindFrameOffset(uint32_t offset, FIL *file) {
	uint32_t index = offset, i, readSize = 0;
	//printf("JPEG_FindFrameOffset file size %lu \n\r", f_size(file));
	do {
		if (f_size(file) <= (index + 1)) {
			// end of file reached
			return 0;
		}
		f_lseek(file, index);
		f_read(file, PatternSearchBuffer, PATTERN_SEARCH_BUFFERSIZE, (UINT*) (&readSize));

		if (readSize != 0) {
			for (i = 0; i < (readSize - 1); i++) {
				if ((PatternSearchBuffer[i] == JPEG_SOI_MARKER_BYTE1) && (PatternSearchBuffer[i + 1] == JPEG_SOI_MARKER_BYTE0)) {
					//printf("Found JPEG_SOI_MARKER at index %lu \n\r", index + i);
					return index + i;
				}
			}

			index += (readSize -1);
		}
	} while (readSize != 0);

	return 0;
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
 *            PLL_P                          = 2
 *            PLL_Q                          = 8
 *            PLL_R                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 6
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	RCC_OscInitStruct.PLL.PLLR = 7;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Activate the OverDrive to reach the 200 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}
}

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void) {
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

/**
 * @brief  On Error Handler.
 * @param  None
 * @retval None
 */
void OnError_Handler(void) {
	BSP_LED_On(LED1);
	printf("!!!!!!!!!! ERROR !!!!!!!!! \n\r");
	while (1) {
		;
	} /* Blocking on error */
}

/**
 * @}
 */

/**
 * @}
 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
