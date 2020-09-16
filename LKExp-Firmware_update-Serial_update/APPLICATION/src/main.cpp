#include "mbed.h"

// DigitalOut led(LED2, 0); // Use LED to visualy confirm the flash of the new application

int main(void)
{
	printf("Hello, I'm application!\n");
    int counter = 0;
    printf("I'm running in a loop.\n");
    while(true){
        counter++;
        printf("%d\n",counter);
        // led = !led;
        ThisThread::sleep_for(1s);
    }
	return 0;
}
