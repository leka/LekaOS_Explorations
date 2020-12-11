#include "mbed.h"
#include "waveplayer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
AUDIO_ApplicationTypeDef appli_state = APPLICATION_IDLE;
FATFS SDFatFs;	/* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */

/* Private function prototypes -----------------------------------------------*/
static void AUDIO_InitApplication(void);
void LCD_SetHeader(uint8_t *header);

int main(void)
{
	/* Init Audio Application */
	AUDIO_InitApplication();

	/* Init TS module */
	BSP_TS_Init(800, 480);

	//##-3- Link the micro SD disk I/O driver ##################################
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		//##-4- Register the file system object to the FatFs module ##############
		if (f_mount(&SDFatFs, (TCHAR const *)SDPath, 0) == FR_OK) { printf("successfully mounted\n"); }
	}

	appli_state = APPLICATION_START;

	/* Run Application (Blocking mode) */
	while (1) {
		appli_state = APPLICATION_READY;

		/* AUDIO Menu Process */
		AUDIO_MenuProcess();
		// printf("AudioState: %d (0=IDLE)\n", AudioState);
	}

	return 0;
}

/*******************************************************************************
							Static Function
*******************************************************************************/

/**
 * @brief  Audio Application Init.
 * @param  None
 * @retval None
 */
static void AUDIO_InitApplication(void)
{
	/* Initialize the LCD */
	BSP_LCD_Init();

	/* LCD Layer Initialization */
	BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

	/* Select the LCD Layer */
	BSP_LCD_SelectLayer(1);

	/* Enable the display */
	BSP_LCD_DisplayOn();

	/* Init the LCD Log module */
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	LCD_SetHeader((uint8_t *)"Audio Playback and Record Application");

	printf("SD library started.\n");

	/* Start Audio interface */

	/* Init Audio interface */
	AUDIO_PLAYER_Init();
}

/**
 * @brief  Display the application header on the LCD screen
 * @param  header: pointer to the string to be displayed
 * @retval None
 */
void LCD_SetHeader(uint8_t *header)
{
	/* Set the LCD Font */
	BSP_LCD_SetFont(&Font16);

	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), Font16.Height * 3);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLUE);

	BSP_LCD_DisplayStringAt(0, Font16.Height, header, CENTER_MODE);

	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font12);
}