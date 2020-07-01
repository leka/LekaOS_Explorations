#include "LekaSD.h"

FATFileSystem leka_fs("leka_fs");

LekaSD::LekaSD() {}

int LekaSD::init() {
	if (0 != sd.init()) {
		printf("Init failed \n");
		return -1;
	}
	printf("Init success \n");

	if (0 != sd.frequency(5000000)) { printf("Error setting frequency \n"); }
	printf("frequency set \n");

	return 0;
}

void LekaSD::displayProperties() {
	printf("sd size: %llu\n", sd.size());
	printf("sd read size: %llu\n", sd.get_read_size());
	printf("sd program size: %llu\n", sd.get_program_size());
	printf("sd erase size: %llu\n", sd.get_erase_size());
}

void LekaSD::displayReadmeFile() {
	// Display the numbers file
	printf("Opening \"/leka_fs/README.txt\"... ");
	fflush(stdout);
	FILE *f = fopen("/leka_fs/README.txt", "r");
	printf("%s\n", (!f ? "Fail :(" : "OK"));
	if (!f) { error("error: %s (%d)\n", strerror(errno), -errno); }

	printf("numbers:\n");
	while (!feof(f)) {
		int c = fgetc(f);
		printf("%c", c);
	}

	printf("\rClosing \"/leka_fs/numbers.txt\"... ");
	fflush(stdout);
	int err = fclose(f);
	printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
	if (err < 0) { error("error: %s (%d)\n", strerror(errno), -errno); }
}

void LekaSD::mount() {
	// Try to mount the filesystem
	printf("Mounting the filesystem... ");
	fflush(stdout);
	int err = leka_fs.mount(&sd);
	printf("%s\n", (err ? "Fail :(" : "OK"));
}

void LekaSD::numbersExample() {
	int err;

	// Open the numbers file
	printf("Opening \"/leka_fs/numbers.txt\"... ");
	fflush(stdout);
	FILE *f = fopen("/leka_fs/numbers.txt", "r+");
	printf("%s\n", (!f ? "Fail :(" : "OK"));
	if (!f) {
		// Create the numbers file if it doesn't exist
		printf("No file found, creating a new file... ");
		fflush(stdout);
		f = fopen("/leka_fs/numbers.txt", "w+");
		printf("%s\n", (!f ? "Fail :(" : "OK"));
		if (!f) { error("error: %s (%d)\n", strerror(errno), -errno); }

		for (int i = 0; i < 10; i++) {
			printf("\rWriting numbers (%d/%d)... ", i, 10);
			fflush(stdout);
			err = fprintf(f, "    %d\n", i);
			if (err < 0) {
				printf("Fail :(\n");
				error("error: %s (%d)\n", strerror(errno), -errno);
			}
		}
		printf("\rWriting numbers (%d/%d)... OK\n", 10, 10);

		printf("Seeking file... ");
		fflush(stdout);
		err = fseek(f, 0, SEEK_SET);
		printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
		if (err < 0) { error("error: %s (%d)\n", strerror(errno), -errno); }
	}

	// Go through and increment the numbers
	for (int i = 0; i < 10; i++) {
		printf("\rIncrementing numbers (%d/%d)... ", i, 10);
		fflush(stdout);

		// Get current stream position
		long pos = ftell(f);

		// Parse out the number and increment
		char buf[BUFFER_MAX_LEN];
		if (!fgets(buf, BUFFER_MAX_LEN, f)) { error("error: %s (%d)\n", strerror(errno), -errno); }
		char *endptr;
		int32_t number = strtol(buf, &endptr, 10);
		if ((errno == ERANGE) ||		   // The number is too small/large
			(endptr == buf) ||			   // No character was read
			(*endptr && *endptr != '\n')   // The whole input was not converted
		) {
			continue;
		}
		number += 1;

		// Seek to beginning of number
		fseek(f, pos, SEEK_SET);

		// Store number
		fprintf(f, "    %d\n", number);

		// Flush between write and read on same file
		fflush(f);
	}
	printf("\rIncrementing numbers (%d/%d)... OK\n", 10, 10);

	// Close the file which also flushes any cached writes
	printf("Closing \"/leka_fs/numbers.txt\"... ");
	fflush(stdout);
	err = fclose(f);
	printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
	if (err < 0) { error("error: %s (%d)\n", strerror(errno), -errno); }
}

void LekaSD::runTest() {
	printf("Starting init...");
	init();
	displayProperties();
	mount();
	numbersExample();
	displayReadmeFile();
}
