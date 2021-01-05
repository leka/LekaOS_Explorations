#include "Audio.h"
#include "mbed.h"

DigitalOut audio_enable(SOUND_ENABLE, 1);
AnalogOut audio_output(MCU_SOUND_OUT);

Audio leka_audio(audio_output, audio_enable);

const char filename[] = "file_example_WAV_5MG.wav";

int main(void)
{
	printf("\nHello, Test of Audio!\n\n");

	leka_audio.playFile(filename);

	return 0;
}
