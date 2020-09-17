#include "mbed.h"

BufferedSerial serial(USBTX, USBRX, 115200);

int main(void) {
    printf("Start a new run!\r\n");
    printf("Program to highlight issue to send data that length is a multiple of 16.\n");
    printf("When it happens, you have to replug the board.\r\n");
    while (true) {
        if (serial.readable()) {
            // Reinitialize buffer to 0
            uint8_t buffer[0x100] = {0};

            //Wait data to be loaded in buffer
            wait_us(25000);
            
            //Read data sent and stock it in buffer
            serial.read(buffer, 0x100);
            
            //Display content of buffer
            printf("Values :");
            for (uint16_t j = 0; j < 0x100; j++) { printf(" %02X", buffer[j]); }
            printf("\r\n");
            
            //Try to flush buffers
            fflush(stdout);
            fflush(stdin);
            serial.sync();
        }
        ThisThread::sleep_for(0.004);
    }
	return 0;
}
