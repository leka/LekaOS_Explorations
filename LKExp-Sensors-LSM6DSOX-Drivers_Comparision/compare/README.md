# Comparison of LSM6DSOX drivers

## Introduction

In order to pick the right driver among proposed list (see README.md in above directory).

Different criteria could be :

* Driver update
* Differences in files
* Modifications to do (mbed compatiblity)

## Presentation of drivers

* `mbed-ST_LSM6DSOX` refer to LSM6DSOX driver on mbed, it is published by ST (manufacturer) [[Link](https://os.mbed.com/teams/ST/code/LSM6DSOX/)].
* `github-STMicroelectronics_STMems_Standard_C_drivers` refer to LSM6DSOX driver on github, it is published by STMicroelectronics (manufacturer) [[Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers)].
* `github-stm32duino_LSM6DSOX` refer to LSM6DSOX driver on github, it is published by stm32duino for Arduino application [[Link](https://github.com/stm32duino/LSM6DSOX)].

## [Criteria] Driver update

Date of writing: 13 May 2020

* `mbed-ST_LSM6DSOX` has 1 commit, the last was 10 July 2019.
* `github-STMicroelectronics_STMems_Standard_C_drivers` has 515 commits, the last was 8 May 2020.
* `github-stm32duino_LSM6DSOX` has 7 commits, the last was 10 March 2020.

## [Criteria] Differences in files
Several files were generated in `compare` folder, they represent differences in code among different drivers files.

Drivers’ names were replaced by numbers in order to simplify:
> 1 : mbed-ST\_LSM6DSOX
> 3 : github-stm32duino\_LSM6DSOX
> 3\_old : github-stm32duino\_LSM6DSOX (25 July 2019)

`_old` refer to a previous version, contemporary of last version of driver on mbed.
`(later)` refer to current version of the driver.

(Diff in color available with Sublime Text software.)

### mbed-ST\_LSM6DSOX and github-STMicroelectronics\_STMems\_Standard\_C\_drivers
#### Ressources
mbed-ST\_LSM6DSOX commit is similar to commit [`88cfaac89e885c2ccbbbf924912230b13be77e4f`](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/commit/88cfaac89e885c2ccbbbf924912230b13be77e4f) of github-STMicroelectronics\_STMems\_Standard\_C\_drivers , the only difference between both is different I3C value in register but don’t have impact on normal running.

Other differences between last version of both drivers can be seen in [history](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/commits/master/lsm6dsox_STdC/driver) (11 commits ago at this time).

#### Major differences
* `LSM6DSOXSensor.*` files missing in github-STMicroelectronics\_STMems\_Standard\_C\_drivers.
* (later) add `group Basic configuration` [[Link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/master/lsm6dsox_STdC/driver/lsm6dsox_reg.c#L9475)] with a lot of new functions and associated `struct`.
* (later) rework `lsm6dsox_all_sources_get()` and all `lsm6dsox_pin_int[1/2]_route_[set/get]()` with all associated `struct`.
* (later) add `struct ucf_line_t` for `.ucf` files (Machine Learning model).
* (later) add `lsm6dsox_timestamp_rst()`.

#### Minor differences
* (later) fix in `lsm6dsox_pin_int2_route_set()`.
* (later) add in `lsm6dsox_xl_data_rate_set()` and `lsm6dsox_gy_data_rate_set()` checking data rate constraints of Finite State Machine (FSM) and Machine Learning Core (MLC).
* (later) remove `LSM6DSOX_PEDO_ADV_MODE`.
* (later) replace `lsm6dsox_ctx_t` by `stmdev_ctx_t`.
* (later) replace `LSM6DSOX_XL_ODR_6Hz5` by `LSM6DSOX_XL_ODR_1Hz6`.
* (later) add `bytecopy()`

### mbed-ST\_LSM6DSOX and github-stm32duino\_LSM6DSOX
#### Ressources

* 1-3\_old (lsm6dsox\_reg\_c).diff
* 1-3\_old (lsm6dsox\_reg\_h).diff
* 1-3\_old (LSM6DSOXSensor\_cpp).diff
* 1-3\_old (LSM6DSOXSensor\_h).diff
* 3\_old-3 (lsm6dsox\_reg\_c).diff
* 3\_old-3 (lsm6dsox\_reg\_h).diff
* 3\_old-3 (LSM6DSOXSensor\_cpp).diff
* 3\_old-3 (LSM6DSOXSensor\_h).diff

#### Major differences
* `MotionSensor` and `GyroSensor` in `LSM6DSOXSensor.*` are mandatory in mbed-ST\_LSM6DSOX.
* `Wire` and `TwoWire` used in github-stm32duino\_LSM6DSOX instead of `DevI2C` in mbed-ST\_LSM6DSOX.
* `Attach()`, `Enable()` and `Disable()` functions are missing for both interrupts (INT1, INT2) in github-stm32duino\_LSM6DSOX.
* In github-stm32duino\_LSM6DSOX, I3C is disabled in `class LSM6DSOXSensor` constructor.
* `SPI` in `LSM6DSOXSensor.*` is mandatory in github-stm32duino\_LSM6DSOX.
* `io_read()` and `io_write()` are slightly different but do same task in both drivers.

#### Minor differences
* `Constructor` of `class LSM6DSOXSensor` in github-stm32duino\_LSM6DSOX is equivalent as `Constructor` + `init()` of `class LSM6DSOXSensor` in mbed-ST\_LSM6DSOX.
* (later) In github-stm32duino\_LSM6DSOX, 2 new functions in `LSM6DSOXSensor.*` act as bridge to existing functions in `lsm6dsox_reg.*` (`Get_MLC_Status()` and `Get_MLC_Output()`)
* `enum LSM6DSOXStatusTypeDef` in github-stm32duino\_LSM6DSOX.
* `float_t` in mbed-ST\_LSM6DSOX replaced by `float` in github-stm32duino\_LSM6DSOX
* Lot of `virtual` functions in mbed-ST\_LSM6DSOX.
* Except functions mention above, all functions are identical but with different names.

## [Criteria] Modifications to do (mbed compatiblity)

* mbed-ST\_LSM6DSOX: fully integrated (driver and above layer) in mbed.
* github-STMicroelectronics\_STMems\_Standard\_C\_drivers: driver is compatible with mbed, not examples that need a BSP. No above layer is provided.
* github-stm32duino\_LSM6DSOX: driver (`lsm6dsox_reg.*`) is compatible but above layer (`LSM6DSOXSensor.*`) is made for arduino and have to be adapted to mbed.

## Conclusion

### Pros & Cons

* mbed-ST\_LSM6DSOX
	* pros
		* ✅ Fully integrated to mbed
	* cons
		* ❌ No update
		* ❌ Dependencies to third libraries (Motion, Gyrosensor, DevI2C, Component)
* github-STMicroelectronics\_STMems\_Standard\_C\_drivers
	* pros
		* ✅ Still update with new functions and fixes
		* ✅ Driver compatible with mbed
	* cons
		* ❌ Missing abstraction layer `LSM6DSOXSensor.*`
		* ❌ Examples need ressources and modifications to be functionnal
* github-stm32duino\_LSM6DSOX
	* pros
		* ✅ I3C disabled at beginning
		* ✅ Additionnal functions for MLC status
	* cons
		* ❌ Modifications of I2C and SPI for mbed compatiblity
		* ❌ Poor update
		* ❌ No interrupt handler

### Suggestion

It is a necessity to stay up to date, so driver `lsm6dsox_reg.*` should be the one provided by manufacturer on github.

A new abstraction layer `LSM6DSOXSensor.*` can be done based on drivers of mbed and stm32duino versions but also recent add `group Basic configuration` on ST version on github.
However only relevant functions should be kept and dependencies should be removed.
