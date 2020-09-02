#include "mbed.h"

int main(void)
{
	printf("Hello, I'm application!\n");
    int counter = 0;
    printf("I'm running in a loop.\n");
    while(true){
        counter++;
        printf("%d\n",counter);
        ThisThread::sleep_for(1s);
    }
	return 0;
}
