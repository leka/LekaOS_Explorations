# LKExp-Bootloader_with_FlashIAP

## Goal

Copy and run a program located further in the flash memory

## Resources

[FlashIAP class reference](https://os.mbed.com/docs/mbed-os/v6.1/mbed-os-api-doxy/classmbed_1_1_flash_i_a_p.html)

## Prerequisite

⚠️ Before running this program, you need to run either **LKExp-Bootloader-DISCO_F769NI-BlinkLED500ms** or **LKExp-Bootloader-DISCO_F769NI-BlinkLED2s** in order to write an app to use in the flash memory.

## Preparation

First you need to set the maximum size of the bootloader at `0x10000` using `"target.restrict_size"` in `mbed_app.json`:

```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```

Then, in `main.cpp`, create a variable of class `FlashIAP`, which you need to initialize using its method `init()`.

Read the flash memory using the method `read(void* buffer, uint32_t addr_of_data_to_read, uint32_t nb_of_bytes_to_read)`, where `addr_of_data_to_read` is either `BLINK_500MS_APP_ADDR` or `BLINK_2S_APP_ADDR`, according to which application you run beforehand.

Use the method `program(void* buffer, uint32_t addr_where_to_write_data, uint32_t nb_of_bytes_to_write)` to write the read data at `POST_APPLICATION_ADDR`.

You can finally call `mbed_start_application(POST_APPLICATION_ADDR)` to launch the application at `POST_APPLICATION_ADDR`, copied from a memory space further in the ROM.
