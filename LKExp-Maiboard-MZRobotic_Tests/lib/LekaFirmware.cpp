#if !DEVICE_QSPI
#error [NOT_SUPPORTED] QSPI not supported for this target
#endif

#include "LekaFirmware.h"
#include <stdio.h>
#include <algorithm>
#include "QSPIFBlockDevice.h"

// #include "mbed.h"
#include "drivers/QSPI.h"

#define CMD_READ           0x03
#define CMD_WRITE          0x02
#define CMD_ERASE          0x20
#define CMD_RDSR           0x5
#define CMD_WREN           0x6
#define CMD_RSTEN          0x66
#define CMD_RST            0x99
#define STATUS_REG_SIZE    2
#define BIT_WIP            0x1
#define BIT_WEL            0x2
#define BUF_SIZE           12

QSPI qspi_device(QSPI_FLASH1_IO0, QSPI_FLASH1_IO1, QSPI_FLASH1_IO2, QSPI_FLASH1_IO3, QSPI_FLASH1_SCK, QSPI_FLASH1_CSN); // io0, io1, io2, io3, sclk, ssel
DigitalOut CE1(PD_12, 1);
DigitalOut CE2(PH_6, 1);
DigitalOut CE3(PJ_4, 1);

LekaFirmware::LekaFirmware(DigitalOut &ce1_disable, DigitalOut &ce2_disable,
						   DigitalOut &ce3_disable)
	: _ce1_disable(ce1_disable), _ce2_disable(ce2_disable), _ce3_disable(ce3_disable) {
	_ce1_disable = 1;
	_ce2_disable = 1;
	_ce3_disable = 1;
}

void LekaFirmware::init() {
	_ce1_disable		  = 0;
    // _ce2_disable		  = 0;


    printf("--- Mbed OS QSPIF block device example ---\n");

    // Initialize the block device
    int err = qspi_bd.init();
    printf("qspi_bd.init -> %d\n", err);

    int erase_val = qspi_bd.get_erase_value();

    // Get device geometry
    bd_size_t read_size    = qspi_bd.get_read_size();
    bd_size_t program_size = qspi_bd.get_program_size();
    bd_size_t erase_size   = qspi_bd.get_erase_size();
    bd_size_t size         = qspi_bd.size();

    printf("--- Block device geometry ---\n");
    printf("read_size:    %lld B\n", read_size);
    printf("program_size: %lld B\n", program_size);
    printf("erase_size:   %lld B\n", erase_size);
    printf("size:         %lld B\n", size);
    printf("---\n");

    // Allocate a block with enough space for our data, aligned to the
    // nearest program_size. This is the minimum size necessary to write
    // data to a block.
    size_t buffer_size = sizeof("Hello Storage!") + program_size - 1;
    buffer_size = buffer_size - (buffer_size % program_size);
    char *buffer = new char[buffer_size];

    // Read what is currently stored on the block device. We haven't written
    // yet so this may be garbage
    printf("qspi_bd.read(%p, %d, %d)\n", buffer, 0, buffer_size);
    err = qspi_bd.read(buffer, 0, buffer_size);
    printf("qspi_bd.read -> %d\n", err);

    printf("--- Stored data ---\n");
    for (size_t i = 0; i < buffer_size; i += 16) {
        for (size_t j = 0; j < 16; j++) {
            if (i + j < buffer_size) {
                printf("%02x ", buffer[i + j]);
            } else {
                printf("   ");
            }
        }

        printf(" %.*s\n", buffer_size - i, &buffer[i]);
    }
    printf("---\n");

    // Update buffer with our string we want to store
    strncpy(buffer, "Hello Storage!", buffer_size);

    // Write data to first block, write occurs in two parts,
    // an erase followed by a program
    printf("qspi_bd.erase(%d, %lld)\n", 0, erase_size);
    err = qspi_bd.erase(0, erase_size);
    printf("qspi_bd.erase -> %d\n", err);

    printf("qspi_bd.program(%p, %d, %d)\n", buffer, 0, buffer_size);
    err = qspi_bd.program(buffer, 0, buffer_size);
    printf("qspi_bd.program -> %d\n", err);

    // Clobber the buffer so we don't get old data
    memset(buffer, 0xcc, buffer_size);

    // Read the data from the first block, note that the program_size must be
    // a multiple of the read_size, so we don't have to check for alignment
    printf("qspi_bd.read(%p, %d, %d)\n", buffer, 0, buffer_size);
    err = qspi_bd.read(buffer, 0, buffer_size);
    printf("qspi_bd.read -> %d\n", err);

    printf("--- Stored data ---\n");
    for (size_t i = 0; i < buffer_size; i += 16) {
        for (size_t j = 0; j < 16; j++) {
            if (i + j < buffer_size) {
                printf("%02x ", buffer[i + j]);
            } else {
                printf("   ");
            }
        }

        printf(" %.*s\n", buffer_size - i, &buffer[i]);
    }
    printf("---\n");

    // Deinitialize the block device
    printf("qspi_bd.deinit()\n");
    err = qspi_bd.deinit();
    printf("qspi_bd.deinit -> %d\n", err);

    printf("--- done! ---\n");



    return;
}

static bool mem_ready()
{
    char status_value[STATUS_REG_SIZE] = {0xFF};
    int retries = 10000;
    bool mem_ready = true;

    do {
        retries--;
        if (QSPI_STATUS_OK != qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE)) {
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

static int write_enable()
{
    char status_value[STATUS_REG_SIZE] = {0};
    int status = -1;

    if (QSPI_STATUS_OK != qspi_device.command_transfer(CMD_WREN, -1, NULL, 0, NULL, 0)) {
        printf("Sending WREN command FAILED \n");
        return status;
    }

    if (false == mem_ready()) {
        printf("Device not ready \n");
        return status;
    }

    if (QSPI_STATUS_OK != qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE)) {
        printf("Reading Status Register failed \n");
        return status;
    }

    if ((status_value[0] & BIT_WEL)) {
        status = 0;
    }
    return status;
}

static int flash_init()
{
    int status = QSPI_STATUS_OK;
    char status_value[STATUS_REG_SIZE] = {0};

    // Read the Status Register from device
    status =  qspi_device.command_transfer(CMD_RDSR, -1, NULL, 0, status_value, STATUS_REG_SIZE);
    if (status != QSPI_STATUS_OK) {
        printf("Reading Status Register failed: value = 0x%x\n", (int)status_value[0]);
        return status;
    }

    // Send Reset Enable
    status = qspi_device.command_transfer(CMD_RSTEN, -1, NULL, 0, NULL, 0);
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
    status = qspi_device.command_transfer(CMD_RST, -1, NULL, 0, NULL, 0);
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

static int sector_erase(unsigned int flash_addr)
{
    if (0 != write_enable()) {
        printf("Write Enabe failed \n");
        return -1;
    }

    if (QSPI_STATUS_OK != qspi_device.command_transfer(CMD_ERASE, (((int)flash_addr) & 0x00FFF000), NULL, 0, NULL, 0)) {
        printf("Erase failed\n");
        return -1;
    }

    if (false == mem_ready()) {
        printf("Device not ready \n");
        return -1;
    }

    return 0;
}

int init(){
    qspi_status_t result;

    result = qspi_device.configure_format(QSPI_CFG_BUS_SINGLE, QSPI_CFG_BUS_SINGLE,
                                          QSPI_CFG_ADDR_SIZE_24, QSPI_CFG_BUS_SINGLE,
                                          QSPI_CFG_ALT_SIZE_8, QSPI_CFG_BUS_SINGLE, 0);
    if (result != QSPI_STATUS_OK) {
        printf("Config format failed\n");
    }

    if (QSPI_STATUS_OK != flash_init()) {
        printf("Init failed\n");
        return -1;
    }

    return 0;
}

int clean_up(uint32_t write_address, uint32_t read_address){

    if (0 != sector_erase(write_address)) {
        return -1;
    }

    if (0 != sector_erase(read_address)) {
        return -1;
    }

    return 0;
}

int ext_flash_write(uint32_t write_address, char tx_buf[], size_t buf_len){
    qspi_status_t result;

    // if (0 != sector_erase(write_address)) {
    //     return -1;
    // }

    if (0 != write_enable()) {
        printf("Write Enabe failed \n");
        return -1;
    }

    result = qspi_device.write(CMD_WRITE, -1, write_address, tx_buf, &buf_len);
    if (result != QSPI_STATUS_OK) {
        printf("Write failed\n");
        return result;
    }
    printf("Write done: %s \n", tx_buf);

    return 0;
}

int ext_flash_read(uint32_t read_address){
    qspi_status_t result;
    char rx_buf[BUF_SIZE] = {0};
    size_t buf_len = sizeof(rx_buf);

    if (false == mem_ready()) {
        printf("Device not ready \n");
        return -1;
    }

    result = qspi_device.read(CMD_READ, -1, read_address, rx_buf, &buf_len);
    if (result != QSPI_STATUS_OK) {
        printf("Read failed\n");
        return result;
    }
    printf("Data Read = %s\n", rx_buf);
    
    return 0;
}

void test_qspi(char index){

    printf("QSPI configuration is IO0: PD11 | IO1: PC10 | IO2: PE2 | IO3: PD13 | SCK: PB2 | CSN: PB10\n");

    // char tx_buf[BUF_SIZE] = { 0x2A, 0x2B, 0x2C, 0x2D };
    char tx_buf[BUF_SIZE] = { 0x30, 0x2D, 0x52, 0x45, 0x41, 0x44, 0x4D, index };
    // char rx_buf[BUF_SIZE] = {0};
    size_t buf_len = sizeof(tx_buf);
    qspi_status_t result;
    uint32_t write_address = 0x1000; //Loop at 0x4000000 (cf datasheet) // Nup, it's limited by address size (24bits or 6 hex digits)
    uint32_t read_address = 0x1000; //0x0001000

    init();

    tx_buf[0]++; 
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (classical)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    read_address = 0x2000;
    printf("\nREAD at 0x%X (read before write)\n", read_address);
    // clean_up(0x2000, 0x2000);
    ext_flash_read(read_address);

    write_address = 0x2000;
    read_address = 0x2006;
    tx_buf[0]++; 
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (offset)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    write_address = 0x3FFA;
    read_address = 0x3FFA;
    tx_buf[0]++;
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (end of Sector)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    read_address = 0x4000;
    printf("\nREAD at 0x%X (next Sector)\n", read_address);
    ext_flash_read(read_address);

    write_address = 0x7FFA;
    read_address = 0x7FFA;
    tx_buf[0]++;
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (end of 32K-byte Block)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    write_address = 0x1FFFFA;
    read_address = 0x1FFFFA;
    tx_buf[0]++;
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (end of 2MB flash on Leka)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    read_address = 0x201000;
    printf("\nREAD at 0x%X (test loop of address on Leka)\n", read_address);
    // clean_up(0x201000, 0x201000);
    ext_flash_read(read_address);

    write_address = 0x205000;
    read_address = 0x205000;
    tx_buf[0]++;
    printf("\nWRITE at 0x%X the message %s, READ at 0x%X (beyond end of 2MB flash on Leka)\n", write_address, tx_buf, read_address);
    clean_up(write_address, read_address);
    ext_flash_write(write_address, tx_buf, buf_len);
    ext_flash_read(read_address);

    printf("\n");
    ThisThread::sleep_for(1s);
}

void LekaFirmware::runTest() {
	printf("\nTest of firmware!\n");
	// init();
        // printf("\nStart run\n");

    printf("Test on CE1\n");
    CE1 = 0;
    test_qspi(0x30);
    ThisThread::sleep_for(3s);

    printf("Test on CE2\n");
    CE1 = 1;
    CE2 = 0;
    test_qspi(0x31);
    ThisThread::sleep_for(3s);

    printf("Test on CE3\n");
    CE2 = 1;
    CE3 = 0;
    test_qspi(0x32);
    ThisThread::sleep_for(3s);

    CE3 = 1;
    printf("End of program\n\r");
    return;
}
