#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

#define FLASH_APP_ADDR 0x10000
#define APP_SIZE 0x10000

QSPI externflash(QSPI_FLASH1_IO0, QSPI_FLASH1_IO1, QSPI_FLASH1_IO2, QSPI_FLASH1_IO3, QSPI_FLASH1_SCK, QSPI_FLASH1_CSN);
FlashIAP MCUflash;

int main(void) {
	
	printf("Starting bootloader\n");
	
	MCUflash.init();
	char* buffer = new char[APP_SIZE];
	size_t data_size = APP_SIZE;
	
	printf("\nStore the current app in external flash instead of copying a new one from it? (Y/n) \n");
	char choice;
	choice = getchar();
	
	if((choice=='Y')||(choice=='y')){
		
		printf("\nCopying program from MCU flash address 0x%x\n", POST_APPLICATION_ADDR);
		if(MCUflash.read(buffer, POST_APPLICATION_ADDR, APP_SIZE))
			printf("Unable to read from MCU flash\n");
		else {
			printf("Storing program into external flash at address 0x%x\n", FLASH_APP_ADDR);
			if(externflash.write(FLASH_APP_ADDR, buffer, &data_size)==QSPI_STATUS_ERROR)
				printf("Unable to write in external flash\n");
			else
				printf("Bytes stored: 0x%x\n", data_size);
		}
		
		printf("Launching current application at address 0x%x\n", POST_APPLICATION_ADDR);
		mbed_start_application(POST_APPLICATION_ADDR);
	}
	
	else {
	
		printf("\nCopying program from external flash address 0x%x\n", FLASH_APP_ADDR);
		if(externflash.read(FLASH_APP_ADDR, buffer, &data_size)==QSPI_STATUS_ERROR)
			printf("Unable to read from external flash\n");
		else {
			printf("0x%x bytes read, writing into MCU flash at address 0x%x\n", data_size, POST_APPLICATION_ADDR);
			if(MCUflash.program(buffer, POST_APPLICATION_ADDR, data_size))
				printf("Unable to write in MCU flash\n");
			else {
				printf("New app successfully copied, launching...\n");
				mbed_start_application(POST_APPLICATION_ADDR);
			}
		}
	}
	
	return 0;
}
