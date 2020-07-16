# LKExp - Flex - Aceltis_Tests

This folder contains operational test code for components of the prototype flex.
Their aim is to test they are operational, **NOT** to test functionalities asked in specs.

⚠️ DISCLAIMER : This is a fast process, the first draft may have inappropriate terms and unsatisfying code architecture.

## List of components (and their main interface)

* LEDs belt (SPI)
* LEDs ears (SPI)
* RFID reader (UART)
* Proximity sensors (I2C)
* Accelerometer (I2C)
* Temperature (I2C)

## Goals

* LEDs (belt & ears)
    * Turn on all LEDs in red, green, blue and white
* RFID reader
    * Detect a tag
* Proximity sensor
    * Detect a hand in approach
* Accelerometer
	* Get the ID.
	* Get (raw) data.
* Temperature
	* Get the ID.
	* Get (raw) data on a ramp of heat.

## Current status of Goal

* [ ] LEDs belt
* [ ] LEDs ears
* [ ] RFID reader
* [ ] Proximity sensors
* [ ] Accelerometer
* [ ] Temperature 
