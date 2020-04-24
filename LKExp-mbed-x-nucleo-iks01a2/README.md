# LKExp-mbed-X\_NUCLEO\_IKS01A2

## Goal

Run mbed examples with X\_NUCLEO\_IKS01A2 board, especially motion MEMS.

## Ressources

[X-NUCLEO-IKS01A2 ST page](https://www.st.com/en/ecosystems/x-nucleo-iks01a2.html#resource)  
[X\_NUCLEO\_IKS01A2 on os.mbed.com](https://os.mbed.com/components/X-NUCLEO-IKS01A2/) 

To display in other IRQ : [Simplify your code with mbed events](https://os.mbed.com/blog/entry/Simplify-your-code-with-mbed-events/)

### LSM6DSOX
To use LSM6DSOX on the STEVAL-MKI197V1 with X-NUCLEO-IKS01A2:

* [LSM6DSOX ST page](https://www.st.com/en/mems-and-sensors/lsm6dsox.html).
	* [LSM6DSOX datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf) to understand how it works.
	* [How to use the sensor on a DIL 24 socket](https://www.st.com/resource/en/application_note/dm00628155-how-to-use-a-sensor-on-a-dil-24-socket-in-xcubemems1-package-applications-stmicroelectronics.pdf) (case of STEVAL-MKI197V1). Application note using LSM6DSO instead of LSM6DSOX but process could be similar.
* [STEVAL-MKI197V1 schematic](https://www.st.com/content/ccc/resource/technical/layouts_and_diagrams/schematic_pack/group0/d3/08/61/50/15/e4/46/7e/STEVAL-MKI197V1_SCHEMATIC/files/steval-mki197v1_schematic.pdf/jcr:content/translations/en.steval-mki197v1_schematic.pdf) to identify pins from LSM6DSOX to X-NUCLEO-IKS01A2.
* [X-NUCLEO-IKS01A2 ST page](https://www.st.com/en/ecosystems/x-nucleo-iks01a2.html).
	* [X-NUCLEO-IKS01A2 UM](https://www.st.com/resource/en/user_manual/dm00333132-getting-started-with-the-xnucleoiks01a2-motion-mems-and-environmental-sensor-expansion-board-for-stm32-nucleo-stmicroelectronics.pdf) to understand how it works.
	* [X-NUCLEO-IKS01A2 schematic](https://www.st.com/content/ccc/resource/technical/layouts_and_diagrams/schematic_pack/group0/e1/7a/8c/96/73/11/4e/0a/X-NUCLEO-IKS01A2%20schematics/files/x-nucleo-iks01a2_schematic.pdf/jcr:content/translations/en.x-nucleo-iks01a2_schematic.pdf) to identify pins from X-NUCLEO-IKS01A2 to "Arduino connector" pins (associated with "Arduino connector" STM32F769I-DISCO for example). Plus understand how to configure hardware to set Mode 2 presented in LSM6DSOX datasheet.
* [LSM6DSOX library under mbed](https://os.mbed.com/teams/ST/code/LSM6DSOX/)