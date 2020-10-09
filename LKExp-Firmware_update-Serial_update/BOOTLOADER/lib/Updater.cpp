
#include "Updater.h"

// BufferedSerial serial(USBTX, USBRX, 9600);

int Updater::receiveFile()
{
	BufferedSerial serial(USBTX, USBRX, 9600);
	DigitalOut CE1(PD_12, 1);

	QspiMemory qspi_memory;

	CE1 = 0;
	qspi_memory.init();

	serial.set_blocking(false);
	uint8_t buffer[0x100] = {0};
	// uint32_t address	  = APPLICATION_ADDR;
	uint32_t application_size = 0x0;
	uint8_t buffer_size		  = 0x8;
	uint32_t qspi_address	  = 0x0;

	if (serial.readable()) {
		// wait_us(1000);
		ThisThread::sleep_for(1ms);
		serial.read(buffer, 0x04);
		application_size = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
		// printf("%X", application_size);
	}
	qspi_memory.sector_erase(qspi_address);
	// wait_us(4000);
	ThisThread::sleep_for(4ms);
	// buffer[0] = (uint8_t)(application_size >> 24);
	// buffer[1] = (uint8_t)(application_size >> 16);
	// buffer[2] = (uint8_t)(application_size >> 8);
	// buffer[3] = (uint8_t)(application_size >> 0);
	qspi_memory.ext_flash_write(0x0, (char *)buffer, 0x4);	 // Write application_size
	buffer[0] = 0xAA;
	qspi_memory.ext_flash_write(0x5, (char *)buffer, 0x1);	 // Write NEW tag
	qspi_address += 0x1000;
	qspi_memory.sector_erase(qspi_address);
	// wait_us(4000);
	ThisThread::sleep_for(4ms);
	int wd = 0;	   // Similar to "Watchdog"
	printf("K");   // ACK to sender
	fflush(stdout);
	wait_us(10);
	while (wd < 0xFF000) {
		if (serial.readable()) {
			// wait_us(25000);
			ThisThread::sleep_for(25ms);
			serial.read(buffer, buffer_size);
			qspi_memory.ext_flash_write(qspi_address, (char *)buffer, buffer_size);
			// wait_us(4000);
			ThisThread::sleep_for(4ms);

			qspi_address += buffer_size;

			if (qspi_address - 0x1000 > application_size) {
				printf("E");   // ACK to sender
				fflush(stdout);
				// wait_us(4000);
				ThisThread::sleep_for(4ms);
				CE1 = 1;
				return 0;
			}
			if (((uint16_t)qspi_address & 0x0FFF) == 0x0000) {
				qspi_memory.sector_erase(qspi_address);
				// wait_us(10000);
				ThisThread::sleep_for(10ms);
			}

			wd = 0;
			printf("K");   // ACK to sender
			fflush(stdout);
			// serial.sync();
		} else {
			wd++;
		}
		// wait_us(4000);	 // 4000 OK, not below
		ThisThread::sleep_for(4ms);
	}
	printf("F");   // NACK to sender
	return -3;
}

void Updater::flashQSPI(uint32_t flash_address)
{
	// BufferedSerial serial(USBTX, USBRX, 9600);
	DigitalOut CE1(PD_12, 1);
	FlashIAP flash;
	QspiMemory qspi_memory;

	uint32_t qspi_address	  = 0x0000;
	uint8_t buffer[0x100]	  = {0};
	uint32_t application_size = 0x0;

	CE1 = 0;

	flash.init();
	qspi_memory.init();

	/* Prior erase flash */
	flash.erase(flash_address, flash.get_sector_size(flash_address));
	ThisThread::sleep_for(1ms);

	// printf("Application size (Main): %lX\n", application_size);
	qspi_memory.ext_flash_get_data(qspi_address, (char *)buffer, (size_t)0x4);
	// printf("Application size (QSPI): %X%X%X%X\n", buffer[0], buffer[1], buffer[2], buffer[3]);
	application_size = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
	qspi_address += 0x1000;	  // First sector dedicated for application information
	while (qspi_address - 0x1000 < application_size) {
		qspi_memory.ext_flash_get_data(qspi_address, (char *)buffer, (size_t)0x100);
		wait_us(200);

		flash.program(buffer, flash_address + qspi_address - 0x1000, 0x100);

		qspi_address += 0x100;
		// printf("%lX\n", qspi_address);
	}

	CE1 = 1;

	return;
}

bool Updater::checkNew()
{
	DigitalOut CE1(PD_12, 1);
	QspiMemory qspi_memory;
	uint8_t buffer[0x1] = {0};

	CE1 = 0;

	qspi_memory.init();

	qspi_memory.ext_flash_get_data(0x5, (char *)buffer, (size_t)0x1);
	if (buffer[0] == 0xAA) {
		buffer[0] = 0x00;
		qspi_memory.ext_flash_write(0x5, (char *)buffer, 0x1);	 // Write OLD tag
		ThisThread::sleep_for(1ms);
		CE1 = 1;
		return true;
	}

	CE1 = 1;
	return false;
}
