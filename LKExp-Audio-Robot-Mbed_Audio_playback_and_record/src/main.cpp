#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "mbed.h"
#include "waveplayer.h"

FATFileSystem SDFatFs("fs"); /* File system object for SD card logical drive */
SDBlockDevice SDBD(SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK);
char SDPath[4]; /* SD card logical drive path */

int main(void)
{
	ThisThread::sleep_for(5s);

	SDBD.init();
	SDBD.frequency(25000000);

	AUDIO_PLAYER_Init();

	SDFatFs.mount(&SDBD);
	printf("SD card mounted!\n");

	/* Run Application (Blocking mode) */
	while (1) { AUDIO_MenuProcess(); }

	return 0;
}
