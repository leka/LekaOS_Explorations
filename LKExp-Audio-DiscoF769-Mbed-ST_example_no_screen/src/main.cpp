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
	printf("SD library started.\n");

	/* Start Audio interface */

	/* Init Audio interface */
	AUDIO_PLAYER_Init();
}
