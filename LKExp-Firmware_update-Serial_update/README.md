# LKExp - Firmware update - Serial update

Related Story : https://github.com/leka/LekaOS/issues/33

## Goal

The goal is to update the application by using the bootloader. Here, the Serial interface will be used to send the new Application.

## Prerequisite

In boot process of microcontroller, bootloader is run first and it is followed by application. When there is a firmware update, only the application is replaced.

## Steps

The results aim is mentioned in the Story mentioned above. They are three steps:

1. Print in terminal when bootloader is running, then print in terminal when application is running.
2. Print in terminal and write the new application when bootloader is running, then print in terminal when application is running.
3. Print in terminal, store the new application in QSPI firmware and write the new application when bootloader is running, then print in terminal when application is running.
