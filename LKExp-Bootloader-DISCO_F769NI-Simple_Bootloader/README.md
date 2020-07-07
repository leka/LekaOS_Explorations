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

Build the bootloader application with the command `mbed compile` and find `<project_name>.bin` in `./BUILD/DISCO_F769NI/GCC_ARM/`, that is the binary image of your bootloader.

## Usage

In a project you want to use the bootloader with, create a folder named `bootloader` and place the bootloader binary image in. Rename it `bootloader.bin` for simplicity.

In the file `mbed_app.json` of this project, add the path of the bootloader image by setting `target.bootloader_img` in the `target_overrides` section:
```
"target_overrides": {
	"*": {
		"target.bootloader_img": "bootloader/bootloader.bin",
		...
	}
```

Make sure the DISCO_F769NI board is connected to your PC over USB and detected. Use the command `mbed compile -f` to build the application with the bootloader and flash it to the board. You can then see your main application run after the bootloader.
