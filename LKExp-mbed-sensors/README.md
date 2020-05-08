# LKExp-mbed-sensors

## Goal

Clean example to use relevant sensors & test differents drivers

Requested in [issue #4](https://github.com/leka/LekaOS_Explorations/issues/4)

## Ressources

[LKExp-mbed-X\_NUCLEO\_IKS01A2](https://github.com/leka/LekaOS_Explorations/tree/master/LKExp-mbed-x-nucleo-iks01a2)

LSM6DSOX:

* mbed by ST : [Link](https://os.mbed.com/teams/ST/code/LSM6DSOX/)
* github by ST : [Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsox_STdC)
* github by stm32duino : [Link](https://github.com/stm32duino/LSM6DSOX)

LSM303AGR (only magnetic sensor)

* mbed by ST : [Link](https://os.mbed.com/teams/ST/code/LSM303AGR/)
* github by ST : [Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm303agr_STdC)
* github by stm32duino : [Link](https://github.com/stm32duino/LSM303AGR)

HTS221

* mbed by ST : [Link](https://os.mbed.com/teams/ST/code/HTS221/)
* github by ST : [Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/hts221_STdC)
* github by stm32duino : [Link](https://github.com/stm32duino/HTS221)

## Construction

* Board set to [BSP\_DISCO\_F769NI](https://os.mbed.com/teams/ST/code/BSP_DISCO_F769NI/) (mbed) but can be changed to any ST board on mbed with Arduino shield connector.
* `DevI2C.h` come from [X\_NUCLEO\_COMMON](https://os.mbed.com/teams/ST/code/X_NUCLEO_COMMON/), it adapts i2c of mbed to i2c to manage i2c devices proposed by ST.
* `Component.h`, `GyroSensor.h`, `HumiditySensor.h`, `MagneticSensor.h`, `MotionSensor.h` and `TempSensor.h` come from [ST\_INTERFACES](https://os.mbed.com/teams/ST/code/ST_INTERFACES/) are abstract classes to build specific drivers.

## Execution

* Plug X\_NUCLEO\_IKS01A2 and STEVAL\_MKI197V1 on Arduino conenctors
* If an error occurs, LED1 turns on (after flash)
