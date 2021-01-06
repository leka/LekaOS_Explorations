#include "Audio.h"

FATFileSystem SDFatFs("fs"); /* File system object for SD card logical drive */
SDBlockDevice SDBD(SD_SPI_MOSI, SD_SPI_MISO, SD_SPI_SCK);

Audio::Audio(AnalogOut &output, DigitalOut &enable) : _output(output), _enable(enable)
{
	SDBD.init();
	SDBD.frequency(25000000);
	SDFatFs.mount(&SDBD);
}

void Audio::playFile(const char *filename)
{
	f_open(&_file, filename, FA_READ);
	printf("File %s openened. File size : %lu bytes\n", filename, getFileSize());

	_ns_sample_rate					= 22676;				 // 1,000,000,000 / 44,100 (in ns)
	uint32_t ns_sample_rate_adapted = _ns_sample_rate / 4;	 // For some reason unknown, sample rate have to be divided
	uint32_t bytesread				= _n_bytes_to_read;

	/* START READ WAV */
	while (bytesread == _n_bytes_to_read) {
		// Read "_n_bytes_to_read" from file at each iteration. Real bytes read is given by "bytesread"
		if (f_read(&_file, &_buffer[0], _n_bytes_to_read, (UINT *)&bytesread) == FR_OK) {
			// Play every 2-bytes (sound encoded in 16 bits)
			for (uint32_t j = 0; j < bytesread; j += 2) {		 // Play one channel, data for stereo are alternate
				_output.write_u16((_buffer[j] + 0x8000) >> 2);	 // offset for int16 data (0x8000) and volume 25% (>>2)
				wait_ns(ns_sample_rate_adapted);				 // adjust play speed
			}
		}
	}
	/* END READ WAV*/

	f_close(&_file);
	printf("File closed\n");
}

uint32_t Audio::getFileSize()
{
	uint32_t file_size = 0;

	if (_file.obj.fs != NULL) { file_size = f_size(&_file); }

	return file_size;
}