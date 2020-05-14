# LKExp-mbed-LSM6DSOX_all_drivers

## Goal

Requested in [issue #6](https://github.com/leka/LekaOS_Explorations/issues/6):

> [...] Build a clean example with mbed and the only the sensor drivers.  
> It would be nice to compare different drivers and see what works best for us.

## Ressources

From [issue #4](https://github.com/leka/LekaOS_Explorations/issues/4):

> - on mbed by ST : [Link](https://os.mbed.com/teams/ST/code/LSM6DSOX/)
> - on github by ST : [Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsox_STdC)
> - on github by stm32duino : [Link](https://github.com/stm32duino/LSM6DSOX)

## How to use

All three share same file name but slighlty different. To avoid error, they are ignored through `.mbedignore` file.  
To enable one of them, just comment with `#` symbol in front of wanted driver.