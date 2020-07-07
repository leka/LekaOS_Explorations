# LKExp-mbed-simple_bootloader

## Goal

Create an application starter, that will launch the main application

## Ressources

GitHub: [Mbed OS bootloader example](https://github.com/ARMmbed/mbed-os-example-bootloader)

Mbed website: [Creating and using a bootloader](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/creating-and-using-a-bootloader.html)

Mbed website: [Bootloader configuration](https://os.mbed.com/docs/mbed-os/v6.0/program-setup/bootloader-configuration.html)

Mbed forum: [Bootloader tutorial not working with certain targets](https://forums.mbed.com/t/bootloader-tutorial-not-working-with-certain-targets/9104)

## Preparation

First you need to define the maximum bootloader size by setting the value of `target.restrict_size` in the `target_overrides` section in `mbed_app.json`:
```
"target_overrides": {
	"*": {
		"target.restrict_size": "0x10000",
		...
	}
```
This allows you to use the `POST_APPLICATION_ADDR` parameter corresponding to the bootloader ending address, which you can use to launch your main application by calling `mbed_start_application(POST_APPLICATION_ADDR);` at the end of the bootloader main function.

Build the bootloader application with `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.
