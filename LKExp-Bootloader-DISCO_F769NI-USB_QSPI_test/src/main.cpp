#include "mbed.h"
#include "QSPI_lib.hpp"

#define EXT_FLASH_ADDR 0x1000

BufferedSerial pc(USBTX,USBRX);
Timer timer;
QSPI extflash(QSPI_FLASH1_IO0, QSPI_FLASH1_IO1, QSPI_FLASH1_IO2, QSPI_FLASH1_IO3, QSPI_FLASH1_SCK, QSPI_FLASH1_CSN);

int main(void) {
	pc.sync();
	printf("USB file reception & QSPI storage\n");
	
	char* message = new char;
	unsigned long i=0;
	
	printf("Waiting for file from USB\n");
	while(!pc.readable());
	
	printf("Receiving file...\n");
	timer.start();
	while(timer.read_ms()<1000) {
		if(pc.readable()) {
			timer.reset();
			pc.read(message+i,1);
			i++;
		}
	}
	timer.stop();
	
	rtos::ThisThread::sleep_for(1000);
	printf("\nCharacters received: %ld, writing to external flash at address 0x%x\n",i,EXT_FLASH_ADDR);
	
    qspi_status_t result = extflash.configure_format(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE,
                                          QSPI_CFG_ADDR_SIZE_24, QSPI_CFG_BUS_SINGLE,
                                          QSPI_CFG_ALT_SIZE_8, QSPI_CFG_BUS_SINGLE, 0);
    if (result != QSPI_STATUS_OK) {
        printf("Config format failed\n");
    }

    if (QSPI_STATUS_OK != flash_init()) {
        printf("Init failed\n");
        return -1;
    }

    if (0 != sector_erase(EXT_FLASH_ADDR)) {
        return -1;
    }

    if (0 != write_enable()) {
        printf("Write Enabe failed \n");
        return -1;
    }
	
	size_t data_size = i;
	if(extflash.write(EXT_FLASH_ADDR, message, &data_size)==QSPI_STATUS_ERROR)
		printf("Unable to write in external flash\n");
	else {
		printf("Bytes stored: %ld\n", data_size);
		delete(message);
		
		printf("Reading again from external flash\n");
		char* buffer = new char[data_size];
		
		if (false == mem_ready()) {
		    printf("Device not ready \n");
		    return -1;
		}
		
		if(extflash.read(EXT_FLASH_ADDR, buffer, &data_size)==QSPI_STATUS_ERROR)
			printf("Unable to read from external flash\n");
		else {
			printf("%ld bytes read, sending back to USB\n",data_size);
			rtos::ThisThread::sleep_for(1000);
			pc.write(buffer,data_size);
		}
	}

	return 0;
}
