# LKExp - Firmware update - Serial update

Related Story : https://github.com/leka/LekaOS/issues/33

## Goal

The goal is to update the application by using the bootloader. Here, the Serial interface will be used to send the new Application.

## Prerequisite

In boot process of microcontroller, bootloader is run first and it is followed by application. When there is a firmware update, only the application is replaced.
Application need to be compiled based on the associated bootloader.

## Steps

The results aim is mentioned in the Story mentioned above. They are three steps:

1. ✅ Print in terminal when bootloader is running, then print in terminal when application is running. Need to use commit [820e155](https://github.com/leka/LekaOS_Explorations/commit/820e155)
2. ✅ Print in terminal and write the new application when bootloader is running; print in terminal when application is running. [1310708](https://github.com/leka/LekaOS_Explorations/pull/17/commits/1310708)
3. Print in terminal, store the new application in QSPI firmware and write the new application when bootloader is running; print in terminal when application is running. [4062c4a](https://github.com/leka/LekaOS_Explorations/pull/17/commits/4062c4a)
4. Print in terminal, start the application if available skip otherwise when bootloader is running; print in terminal, store the new application in QSPI firmware, write the new application and reboot when application is running.

## How to use

* Step 1 : Run flash.zsh script.
* Step 2 : Compile the bootloader and flash it, compile the application (DO NOT FLASH IT!), run the python script
* Step 3 : Compile the bootloader and flash it, compile the application (DO NOT FLASH IT!), run the python script

## ⚠️ Potential issues

* [Step 2] Oversized Application is not handle yet, their could be an error if its size is higher than secor size.
* [Step 2] Serial is mandatory by the python script. If you want to see it in a terminal, you have to free it before writing and connect again after the script print `Finished send`
* [Step 2] The success rate of sending is not 100%. In case of error happens, restart target and rerun the python script. It happens at the start of the sending process.
* [Step 3] Verified on Leka board, not on DiscoF769.