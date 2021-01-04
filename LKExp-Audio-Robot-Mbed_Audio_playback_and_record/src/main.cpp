#include "mbed.h"
#include "waveplayer.h"

FATFS SDFatFs;	/* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */

int main(void)
{
	/* Init Audio Application */
	AUDIO_PLAYER_Init();

	//##-3- Link the micro SD disk I/O driver ##################################
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		//##-4- Register the file system object to the FatFs module ##############
		if (f_mount(&SDFatFs, (TCHAR const *)SDPath, 0) == FR_OK) { printf("successfully mounted\n"); }
	}

	/* Run Application (Blocking mode) */
	while (1) { AUDIO_MenuProcess(); }

	return 0;
}
