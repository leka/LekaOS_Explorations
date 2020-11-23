#include "QspiMemory.h"
#include "drivers/QSPI.h"
#include "mbed.h"

#ifndef POST_APPLICATION_ADDR
#error "target.restrict_size must be set for your target in mbed_app.json"
#endif

#if !DEVICE_QSPI
#error[NOT_SUPPORTED] QSPI not supported for this target
#endif

#define APPLICATION_ADDR 0x08020000
#define WRITE_QSPI 1
#define USE_HEX_FILE 0
#define HEX_FILE_LINE_MAX_LENGTH 0x15	// 0x15 (21) = 0x10 (16) bytes of data + 0x05 (5) bytes of informations

DigitalOut CE1(PD_12, 1);
BufferedSerial serial(USBTX, USBRX, 115200);
FlashIAP flash;
bool enable_programming = true;
QspiMemory qspi_memory;
uint32_t application_size = 0x0;

int binFileTransfer()
{
	serial.set_blocking(false);
	uint8_t buffer[0x100]	  = {0};
	uint32_t address		  = APPLICATION_ADDR;
	// uint32_t application_size = 0x0;
	uint8_t buffer_size		  = 0x8;
	uint32_t qspi_address	  = 0x0;

	if (serial.readable()) {
		wait_us(1000);
		serial.read(buffer, 0x04);
		application_size = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
		// printf("%X", application_size);
	}
	if (WRITE_QSPI) {
		qspi_memory.sector_erase(qspi_address);
		wait_us(4000);
		// buffer[0] = (uint8_t)(application_size >> 24);
		// buffer[1] = (uint8_t)(application_size >> 16);
		// buffer[2] = (uint8_t)(application_size >> 8);
		// buffer[3] = (uint8_t)(application_size >> 0);
		qspi_memory.ext_flash_write(0x0, (char *)buffer, 0x4);	 // Write application_size
		qspi_address += 0x1000;
		qspi_memory.sector_erase(qspi_address);
		wait_us(4000);
	}
	int wd = 0;	   // Similar to "Watchdog"
	printf("K");   // ACK to sender
	fflush(stdout);
	wait_us(10);
	while (wd < 0xFF000) {
		if (serial.readable()) {
			wait_us(25000);
			serial.read(buffer, buffer_size);
			if (enable_programming) {
				if (WRITE_QSPI) {
					qspi_memory.ext_flash_write(qspi_address, (char *)buffer, buffer_size);
					wait_us(4000);

					qspi_address += buffer_size;

					if (qspi_address - 0x1000 > application_size) {
						printf("E");   // ACK to sender
						fflush(stdout);
						wait_us(4000);
						// ThisThread::sleep_for(0.004);
						return 0;
					}
					if (((uint16_t)qspi_address & 0x0FFF) == 0x0000) {
						qspi_memory.sector_erase(qspi_address);
						wait_us(10000);
					}

				} else {
					flash.program(buffer, address, buffer_size);
					address += buffer_size;

					if (address - APPLICATION_ADDR > application_size) {
						printf("E");   // ACK to sender
						fflush(stdout);
						wait_us(4000);
						// ThisThread::sleep_for(0.004);
						return 0;
					}
				}
				wd = 0;
				printf("K");   // ACK to sender
				fflush(stdout);
				// serial.sync();
			}
		} else {
			wd++;
		}
		wait_us(4000);	 // 4000 OK, not below
						 // ThisThread::sleep_for(0.004*16);
	}
	printf("F");   // NACK to sender
	return -3;
}

int hexFileTransfer()
{
	serial.set_blocking(false);
	uint8_t buffer[0x15]			= {0};
	uint8_t buffer_size				= 0x0;
	uint8_t checksum				= 0x00;
	uint32_t address				= 0x00000000;
	uint16_t address_end			= 0x00000000;
	uint32_t qspi_address			= 0x00000000;
	uint8_t qspi_to_end_sector_size = 0x0;	 // Could be larger than 0x00 if data transfer is superior to 256 bytes. But
											 // keep an eye on sector size for programming.

	if (WRITE_QSPI) {
		qspi_memory.sector_erase(qspi_address);
		wait_us(400000);
		qspi_memory.sector_erase(qspi_address+0x1000);
		wait_us(400000);
	}

	int wd = 0;	   // Similar to "Watchdog"
	printf("K");   // ACK to sender
	fflush(stdout);
	wait_us(10);
	while (wd < 0xFF0000) {
		if (serial.readable()) {
			serial.read(buffer, 0x01);
			if (buffer[0] == 0xFF) {
				wait_us(4000);
				continue;
			}
			buffer_size = buffer[0];
			serial.read(buffer + 1, 0x04 + buffer_size);
			// wait_us(1);

			checksum = 0;
			for (int i = 0; i < 0x05 + buffer_size; i++) { checksum += buffer[i]; }
			if (checksum != 0x00) { return -1; }

			// https://en.wikipedia.org/wiki/Intel_HEX#Record_types
			if (buffer[3] == 0x00) {
				address = (address & 0xFFFF0000) | buffer[1] << 8 | buffer[2];
				if (enable_programming) {
					if (WRITE_QSPI) {
						qspi_address = address - APPLICATION_ADDR + 0x1000;
						address_end	 = ((uint16_t)address % 0x1000) + buffer_size;
						if (address_end < 0x1000) {
							qspi_memory.ext_flash_write(qspi_address, (char *)buffer + 0x04, buffer_size);
						} else if (address_end == 0x1000) {
							qspi_memory.ext_flash_write(qspi_address, (char *)buffer + 0x04, buffer_size);
							wait_us(400000);

							qspi_address += buffer_size;
							qspi_memory.sector_erase(qspi_address + 0x1000);
							wait_us(1000000);
						} else {
							qspi_to_end_sector_size = buffer_size - (address_end - 0x1000);
							qspi_memory.ext_flash_write(qspi_address, (char *)buffer + 0x04, qspi_to_end_sector_size);

							qspi_address += qspi_to_end_sector_size;
							qspi_memory.sector_erase(qspi_address);
							wait_us(400000);

							qspi_memory.ext_flash_write(qspi_address, (char *)buffer + 0x04 + qspi_to_end_sector_size,
														buffer_size - qspi_to_end_sector_size);
						}
					} else {
						flash.program(buffer + 0x04, address, buffer_size);
					}
				}
			} else if (buffer[3] == 0x01) {
				application_size =
					address + 0x10 - APPLICATION_ADDR;	 // 0x10 is the largest size possible of the last line, but it
														 // need to be adapted with the real value
				if (WRITE_QSPI){
					buffer[0] = (uint8_t)(application_size >> 24);
					buffer[1] = (uint8_t)(application_size >> 16);
					buffer[2] = (uint8_t)(application_size >> 8);
					buffer[3] = (uint8_t)(application_size >> 0);
					qspi_memory.ext_flash_write(0x0, (char *)buffer, 0x4);	 // Write application_size
				}
				printf("E");							 // ACK to sender
				fflush(stdout);
				wait_us(4000);
				return 0;	// END OF FILE
			} else if (buffer[3] == 0x04) {
				address = (uint32_t)((uint32_t)buffer[4] << 24 | (uint32_t)buffer[5] << 16);
			} else if (buffer[3] == 0x05) {
				// printf("Starting address (?) : %X%X%X%X\n", buffer[4], buffer[5], buffer[6], buffer[7]);
				wait_us(10);
			} else {
				printf("F");   // NACK to sender
				return -2;
			}
			// printf("Address is : %X\n", address);
			wd = 0;
			printf("K");   // ACK to sender
			fflush(stdout);
		} else {
			wd++;
		}
		wait_us(4000);	 // 4000 OK, not below
	}
	printf("F");   // NACK to sender
	return -3;
}

void printCheckErasing()
{
	uint8_t check_buffer[HEX_FILE_LINE_MAX_LENGTH] = {0};

	flash.read(check_buffer, APPLICATION_ADDR, 0x15);
	printf("Check good erasing of this region\n");
	for (uint32_t i = 0; i < 0x11; i++) {
		if (check_buffer[i] != 0xFF) {
			printf("Address : %lX | Value : %X\n", APPLICATION_ADDR + i, check_buffer[i]);
			wait_us(1);
			// fflush(stdout);
		}
	}
	printf("\n");
}

void printCheckProgramming()
{
	uint8_t check_buffer[HEX_FILE_LINE_MAX_LENGTH] = {0};

	printf("Check good programming at beginning of this region\n");
	for (uint32_t i = 0; i < 0x100; i += 0x10) {
		flash.read(check_buffer, APPLICATION_ADDR + i, 0x10);
		printf("Address : %lX | Value :", APPLICATION_ADDR + i);
		for (uint8_t j = 0; j < 0x10; j++) {
			printf(" %02X", check_buffer[j]);	// https://github.com/ARMmbed/mbed-os/issues/12718
		}
		printf("\n");
		wait_us(100);
	}
}

int main(void)
{
	bool update_data_available = false;
	int i					   = 1;
	CE1						   = 0;

	flash.init();
	if (WRITE_QSPI) { qspi_memory.init(); }

	printf("Hello, I'm the bootloader!\n");
	printf("I'm waiting for an application to be sent to me...");
	fflush(stdout);
	update_data_available = serial.readable();

	while (!update_data_available && i < 101) {
		ThisThread::sleep_for(1s);
		printf("\n %d...", i++);
		fflush(stdout);
		update_data_available = serial.readable();
	}
	printf("\n\n");

	if (update_data_available) {
		if (WRITE_QSPI) {
			printf("An application is available! I'm writing the application to QSPI flash!\n\n");
		} else {
			printf("An application is available! I'm writing the application to flash!\n\n");
		}

		// printf("Sector size: %X\n", flash.get_sector_size(0x08020000)); //Return : 0x20000
		// printf("Next sector is at 0x%X\n", APPLICATION_ADDR+flash.get_sector_size(APPLICATION_ADDR));

		/* Erase prior program */
		// printf("Erase section first...\n");
		flash.erase(APPLICATION_ADDR, flash.get_sector_size(APPLICATION_ADDR));
		wait_us(1000);

		/* Check good erase */
		// printCheckErasing()

		/* Program all */
		int err = 0;
		if (USE_HEX_FILE) {
			err = hexFileTransfer();
		} else {
			err = binFileTransfer();
			// err = 0;
		}

		ThisThread::sleep_for(5s);

		if (err != 0) {
			printf("An error occurs : %d\n", err);
		} else {
			// printf("Everything fine!\n");
			if (WRITE_QSPI) {
				printf("Application written to QSPI!\n\n");
			} else {
				printf("Application written!\n\n");
			}
		}

		/* Flash content of external firmware in QSPI to the flash */
		if (WRITE_QSPI) {
			printf("Now I'm written the application to the MCU...\n\n");

			uint32_t qspi_address = 0x0000;
			uint8_t buffer[0x100]  = {0};

			printf("Application size (Main): %lX\n", application_size);
			qspi_memory.ext_flash_get_data(qspi_address, (char *)buffer, (size_t)0x4);
			printf("Application size (QSPI): %X%X%X%X\n", buffer[0], buffer[1], buffer[2], buffer[3]);
			qspi_address+=0x1000;
			while (qspi_address - 0x1000 < application_size) {
				qspi_memory.ext_flash_get_data(qspi_address, (char *)buffer, (size_t)0x100);
				wait_us(200);

				flash.program(buffer, APPLICATION_ADDR + qspi_address - 0x1000, 0x100);

				qspi_address += 0x100;
				printf("%lX\n", qspi_address);
			}

			printf("Application written to MCU.\n\n");
		}
		CE1 = 1;

		/* Visual check of good programming */
		// printCheckProgramming();

		printf("Starting application now!\n\n\n");
		wait_us(2000);
		mbed_start_application(POST_APPLICATION_ADDR);
	} else {
		printf("No application available.\nEnd of the program.\n\n\n");
		wait_us(2000);
	}

	return 0;
}
