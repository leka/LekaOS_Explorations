#include "LekaFirmware.h"
#include <stdio.h>
#include <algorithm>
#include "QSPIFBlockDevice.h"

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
void LekaFirmware::runTest() {
	printf("\nTest of firmware!\n");
	init();
	printf("\n");
    return;
}
