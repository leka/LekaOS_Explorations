# LKExp - Mainboard - MZRobotic_Tests

This folder contains operational test code for components of the prototype mainboard.
Their aim is to test they are operational, **NOT** to test functionalities asked in specs.

⚠️ DISCLAIMER : This is a fast process, the first draft may have inappropriate terms and unsatisfying code architecture.

## List of components (and their main interface)

* Firmwares (QSPI)
* SD (SPI)
* SRAM
* Screen (DSI)
* Accelerometer & Gyroscope (I2C)
* Magnetometer (I2C)
* Temperature (I2C)
* Microphone (Analog)
* Light sensor (Analog)
* Motors (PWM)
* BLE (SPI)
* Wifi (USART)
* Bluetooth (UART)
* Speakers (Analog)

## Goals

* Firmware
	* Write, shutdown/reboot, Read. Written content must be the same on read.
* SD
	* Read existing (text) file in SD card.
* SRAM
	* *To define*
* Screen
	* *To define*
* Accelerometer & Gyroscope
	* Get the ID.
	* Get (raw) data.
* Magnetometer
	* Get the ID.
	* Get (raw) data.
* Temperature
	* Get the ID.
	* Get (raw) data on a ramp of heat.
* Microphone
	* Get (raw) data on a ramp of sound.
* Light sensor
	* Get (raw) data on a ramp of light.
* Motors
	* Turn both motors
	* Ramp from one direction to another to maximale speed.
* BLE
	* *To define*
* Wifi
	* *To define*
* Bluetooth
	* *To define*
* Speakers
    * Play one frequency e.g. 440Hz
	* Play frequencies from 20Hz to 20kHz.

## Current status of Goal

* [ ] Firmwares
* [ ] SD
* [ ] SRAM
* [ ] Screen ❌
* [x] Accelerometer & Gyroscope ✅
* [ ] Magnetometer
* [ ] Temperature
* [x] Microphone ✅
* [x] Light sensor ✅
* [x] Motors ✅
* [ ] BLE
* [ ] Wifi
* [ ] Bluetooth
* [x] Speakers ✅