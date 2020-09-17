#if !DEVICE_QSPI
#error[NOT_SUPPORTED] QSPI not supported for this target
#endif

#include "QspiMemory.h"

QspiMemory::QspiMemory(PinName qspi_flash_io0, PinName qspi_flash_io1, PinName qspi_flash_io2, PinName qspi_flash_io3,
					   PinName qspi_flash_sck, PinName qspi_flash_csn)
	: _qspi_device(qspi_flash_io0, qspi_flash_io1, qspi_flash_io2, qspi_flash_io3, qspi_flash_sck, qspi_flash_csn)
{
}

bool QspiMemory::mem_ready()
{
	char status_value[STATUS_REG_SIZE] = {0xFF};
	int retries						   = 10000;
	bool mem_ready					   = true;

	do {
		retries--;
		if (QSPI_STATUS_OK != _qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE)) {
			printf("Reading Status Register failed \n");
		}
		ThisThread::sleep_for(1);
	} while ((status_value[0] & BIT_WIP) != 0 && retries);

	if ((status_value[0] & BIT_WIP) != 0) {
		printf("mem_ready FALSE: status value = 0x%x\n", (int)status_value[0]);
		mem_ready = false;
	}
	return mem_ready;
}

int QspiMemory::write_enable()
{
	char status_value[STATUS_REG_SIZE] = {0};
	int status						   = -1;

	if (QSPI_STATUS_OK != _qspi_device.command_transfer(CMD_WREN, -1, NULL, 0, NULL, 0)) {
		printf("Sending WREN command FAILED \n");
		return status;
	}

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return status;
	}

	if (QSPI_STATUS_OK != _qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE)) {
		printf("Reading Status Register failed \n");
		return status;
	}

	if ((status_value[0] & BIT_WEL)) { status = 0; }
	return status;
}

int QspiMemory::flash_init()
{
	int status						   = QSPI_STATUS_OK;
	char status_value[STATUS_REG_SIZE] = {0};

	// Read the Status Register from device
	status = _qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE);
	if (status != QSPI_STATUS_OK) {
		printf("Reading Status Register failed: value = 0x%x\n", (int)status_value[0]);
		return status;
	}

	// Send Reset Enable
	status = _qspi_device.command_transfer(CMD_RSTEN, -1, NULL, 0, NULL, 0);
	if (status == QSPI_STATUS_OK) {
		printf("Sending RSTEN Success \n");
	} else {
		printf("Sending RSTEN failed \n");
		return status;
	}

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return -1;
	}

	// Send Reset
	status = _qspi_device.command_transfer(CMD_RST, -1, NULL, 0, NULL, 0);
	if (status == QSPI_STATUS_OK) {
		printf("Sending RST Success \n");
	} else {
		printf("Sending RST failed \n");
		return status;
	}

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return -1;
	}
	return status;
}

int QspiMemory::sector_erase(unsigned int flash_addr)
{
	if (0 != write_enable()) {
		// printf("Write Enabe failed \n");
		return -1;
	}

	if (QSPI_STATUS_OK !=
		_qspi_device.command_transfer(CMD_ERASE, (((int)flash_addr) & 0x00FFF000), NULL, 0, NULL, 0)) {
		printf("Erase failed\n");
		return -1;
	}

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return -1;
	}

	return 0;
}

int QspiMemory::init()
{
	qspi_status_t result;

	result = _qspi_device.configure_format(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE, QSPI_CFG_ADDR_SIZE_24,
										   QSPI_CFG_BUS_SINGLE, QSPI_CFG_ALT_SIZE_8, QSPI_CFG_BUS_SINGLE, 0);
	if (result != QSPI_STATUS_OK) { 
		printf("Config format failed\n"); 
	}

	if (QSPI_STATUS_OK != flash_init()) {
		printf("Init failed\n");
		return -1;
	}

	return 0;
}

int QspiMemory::ext_flash_write(uint32_t write_address, char tx_buf[], size_t buf_len)
{
	qspi_status_t result;

	// if (0 != sector_erase(write_address)) { return -1; }

	if (0 != write_enable()) {
		printf("Write Enabe failed \n");
		return -1;
	}

	result = _qspi_device.write(CMD_WRITE, -1, write_address, tx_buf, &buf_len);
	if (result != QSPI_STATUS_OK) {
		printf("Write failed\n");
		return result;
	}
	// printf("Write done: %s \n", tx_buf);

	return 0;
}

int QspiMemory::ext_flash_read(uint32_t read_address)
{
	qspi_status_t result;
	char rx_buf[BUF_SIZE] = {0};
	size_t buf_len		  = sizeof(rx_buf);

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return -1;
	}

	result = _qspi_device.read(CMD_READ, -1, read_address, rx_buf, &buf_len);
	if (result != QSPI_STATUS_OK) {
		printf("Read failed\n");
		return result;
	}
	printf("Data Read = %s\n", rx_buf);

	return 0;
}

int QspiMemory::ext_flash_read_bis(uint32_t read_address, char rx_buf[], size_t buf_len)
{
	qspi_status_t result;

	if (false == mem_ready()) {
		printf("Device not ready \n");
		return -1;
	}

	result = _qspi_device.read(CMD_READ, -1, read_address, rx_buf, &buf_len);
	if (result != QSPI_STATUS_OK) {
		printf("Read failed\n");
		return result;
	}

	// printf("   %04X  ", read_address);
	// for (uint8_t j = 0; j < buf_len; j++) {
	// 	printf("%02X", rx_buf[j]); 
	// }
	// printf("\n");
	wait_us(100);

	return 0;
}

void QspiMemory::runTest()
{
	printf("\nTest of firmware!\n");

	char tx_buf[BUF_SIZE] = {0x30, 0x2D, 0x52, 0x45, 0x41, 0x44, 0x4D};
	size_t buf_len		  = sizeof(tx_buf);
	// qspi_status_t result;
	uint32_t write_address =
		0x1000;	  // Loop at 0x4000000 (cf datasheet) // Nup, it's limited by address size (24bits or 6 hex digits)
	uint32_t read_address = 0x1000;	  // 0x0001000

	init();

	write_address = 0x3FFA;
	read_address  = 0x3FFA;
	tx_buf[0]++;
	printf("\nWRITE at 0x%X the message %s, READ at 0x%X (end of Sector)\n", write_address, tx_buf, read_address);
	// sector_erase(0x3000);
	sector_erase(0x4000);
	ext_flash_write(write_address, tx_buf, buf_len);
	ext_flash_read(read_address);

	read_address = 0x4000;
	printf("\nREAD at 0x%X (next Sector)\n", read_address);
	ext_flash_read(read_address);

	printf("\n");
	ThisThread::sleep_for(1s);
	printf("End of program\n\r");
	return;
}
