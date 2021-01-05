#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "mbed.h"

class Audio
{
  public:
	Audio(AnalogOut &output, DigitalOut &enable);

	void playFile(const char *filename);
	uint32_t getFileSize();

  private:
	AnalogOut &_output;
	DigitalOut &_enable;

	uint32_t _ns_sample_rate;

	FIL _file;
	static const int _n_bytes_to_read = 1024;	// arbitrary
	int16_t _buffer[_n_bytes_to_read];
};
