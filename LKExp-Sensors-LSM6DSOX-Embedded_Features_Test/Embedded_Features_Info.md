# Embedded Features Documentation

This document is used as a "dump" of all the things we are learning about the LSM6DSOX embedded features.

## Information sources

#### LSM6DSOX and its MLC
* **LSM6DSOX Datasheet** : [link](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf) iNEMO inertial module: always-on 3D accelerometer and 3D gyroscope
* **AN5272** : [link](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf) LSM6DSOX: always-on 3D accelerometer and 3D gyroscope
* **AN5259** : [link](https://www.st.com/content/ccc/resource/technical/document/application_note/group1/5f/d8/0a/fe/04/f0/4c/b8/DM00563460/files/DM00563460.pdf/jcr:content/translations/en.DM00563460.pdf) LSM6DSOX: Machine Learning Core

#### Hardware
* **UM2435** [link](https://www.st.com/resource/en/user_manual/dm00517423-bluetooth-low-energy-and-802154-nucleo-pack-based-on-stm32wb-series-microcontrollers-stmicroelectronics.pdf) : P-NUCLEO-WB55 user manual
* **UM2121** [link](https://www.st.com/resource/en/user_manual/dm00333132-getting-started-with-the-xnucleoiks01a2-motion-mems-and-environmental-sensor-expansion-board-for-stm32-nucleo-stmicroelectronics.pdf) : X-NUCLEO-IKS01A2 user manual
* STEVAL-MKI197V1 : Data brief (no reference)

#### Unanswered questions


#### Software examples (from ST)


## General info

## Important info to be able to work
* the INT1 pin on the LSM6DSOX has to be set at 0 at startup for the I2C not to be disabled. If the board hasn't a pull-down on that pin, you have to set it as a DigitalOut LOW (if you want to use INT1, set it as a DigitalInOut, output, low. Then once the sensor has started, disable I3C via its internal functions to force I2C and then set INT1 pin as input)

* After the device is powered up, it performs a 10 ms (maximum) boot procedure. During the boot time the registers are not accessible.

* INT2 signals can be redirected on INT1

* interrupts on the MLC are part of *embedded fucntions* interrupts, their behavior (latched/pulsed) is controlled by **PAGE_RW** register.


## Configure Embedded features


### 1. Activity detect

### 2. Free fall

Free Fall embedded feature allows to detect when the device is in free fall.
This corresponds to the norm of the acceleration being at 0, in theory, and in a *free fall zone* around 0 in practice.

Two parameters can be modified to configure free fall
* A threshold defining the *free fall zone* amplitude
* A minimum duration before activating the free fall signal

This table shows the value of the *free fall zone* threshold depending on the values of the configuration register
FREE_FALL - FF_THS[2:0] | Threshold LSB value [mg]
--- | ---
000 | 156
001 | 219
010 | 250
011 | 312
100 | 344
101 | 406
110 | 469
111 | 500

#### Configuration registers
**FREE_FALL**
FREE_FALL /
WAKE_UP_DUR
FF_DUR[5:0]
N/ODR_XL
#### Interrupt registers
INTERRUPTS_ENABLE bit in the TAP_CFG2 reg
INT1_FF bit of the MD1_CFG register to 1 or the
INT2_FF bit of the MD2_CFG register to 1; it can also be checked by reading the FF_IA bit of the
WAKE_UP_SRC register.
### 3. Orientation

### 4. Tap (single and double)

### 5. Tilt

### 6. Wake up





## Associated registers

* **MLC_STATUS_MAINPAGE** (38h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=76) page 76 
    * Interrupt status for each tree

* **EMB_FUNC_EN_B** (05h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=111) page 111
    * MLC_EN : Enable the MLC 
    
* **MLC_INT1** (0Dh) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=115) page 115
    * Each bit in this register enables a signal to be carried through INT1. The pin's output will supply the OR combination of the selected signals.

* **MLC_INT2** (11h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=119) page 119
    * Each bit in this register enables a signal to be carried through INT2. The pin's output will supply the OR combination of the selected signals.

* **MLC_STATUS** (15h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=123) page 123
    * Interrupt status for each tree

* **EMB_FUNC_ODR_CFG_C** (60h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=143) page 143
    * MLC_ODR [1:0] : Configure the MLC data rate
    
* **EMB_FUNC_INIT_B** (67h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=145) page 145
    *  MLC_INIT : MLC Initialization request

* **MLC*x*_SRC** (70h-77h): [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=145) page 145
    * Output value of MLC_x_ decision tree (*x* goes from 0 to 7)

## Interrupts
* **INT1_CTRL** (0Dh) and **INT2_CTRL** (0Eh) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=54) page 54 et 55
    * INT1 pin control register (r/w)
    Each bit in this register enables a signal to be carried out on INT1 when the I2C or SPI is used. The output of the pin will be the OR combination of the signals selected here and in MD1_CFG (5Eh).

* **CTRL3_C** (12h): [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=58) page 58
    * H_LACTIVE : define the output state of the interrupt pin on activation(High or Low)
    * PP_OD : push-pull or open drain mode on INT1 and INT2 (Push pull : Active is pushed to Vcc and inactive pulled down to Ground. Open drain : Only the pulling to the ground part is done, the active state is floating and not defined)

* **CTRL4_C** (13h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=59) page 59
    * INT2_on_INT1: This bit allows to reroute interrupts on INT2 towards INT1
    
* **ALL_INT_SRC** (1Ah): [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=66) page 66
    * Reading this register or the related source register will reset the latched interrupt

* **TAP_CFG0** (56h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=82) page 82
    * INT_CLR_ON_READ : This bit allows immediately clearing the latched interrupts of an event detection upon the read of the corresponding status register. It must be set to 1 together with LIR
    * LIR : latch basic interrupts, only usefull for basic interrupts, for MLC interrupts see **PAGE_RW** (17h)

* **TAP_CFG2** (58h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=83) page 83
    * INTERRUPTS_ENABLE : Enable basic interrupts (on embedded functions)
    
* **MD1_CFG** (5Eh) and **MD2_CFG** (5Fh) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=88) page 88 et 89
    * INT*x*_EMB_FUNC (*x* = 1 or 2) : Embedded functions routing on INT1 and INT2 respectively

* **PAGE_RW** (17h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=124) page 124
    * EMB_FUNC_LIR : Latched Interrupt mode for Embedded Functions (this includes the MLC)
