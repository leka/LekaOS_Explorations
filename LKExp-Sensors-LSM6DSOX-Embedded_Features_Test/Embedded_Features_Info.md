# Embedded Features Documentation

This document is used as a "dump" of all the things we are learning about the LSM6DSOX embedded features.

## Information sources

#### LSM6DSOX and its MLC
* **LSM6DSOX Datasheet** : [link](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf) iNEMO inertial module: always-on 3D accelerometer and 3D gyroscope
* **AN5272** : [link](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf) LSM6DSOX: always-on 3D accelerometer and 3D gyroscope



#### Unanswered questions


#### Software examples (from ST)


## General info

## Important info to be able to work
* the INT1 pin on the LSM6DSOX has to be set at 0 at startup for the I2C not to be disabled. If the board hasn't a pull-down on that pin, you have to set it as a DigitalOut LOW (if you want to use INT1, set it as a DigitalInOut, output, low. Then once the sensor has started, disable I3C via its internal functions to force I2C and then set INT1 pin as input)

* After the device is powered up, it performs a 10 ms (maximum) boot procedure. During the boot time the registers are not accessible.

* INT2 signals can be redirected on INT1

* interrupts on the MLC are part of *embedded fucntions* interrupts, their behavior (latched/pulsed) is controlled by **PAGE_RW** register.


## Configure Embedded features


### 1. Activity detection



### 2. Free fall

Free Fall embedded feature allows to detect when the device is in free fall.
This corresponds to the acceleration on all axis being at 0, in theory, and in a *free fall zone* around 0 in practice.

Two parameters can be modified to configure free fall
* A threshold defining the *free fall zone* amplitude
* A minimum duration before activating the free fall signal

This table shows the value of the *free fall zone* threshold depending on the values of the configuration register. The higher the value of the threshold, the bigger *free fall zone* becomes. The signal will be easier to trigger but but there is a higher risk of false positives.
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

The free fall duration  is set up with the formula : time = N/ODR_XL
* ODR_XL is the accelerometer data rate
* N is a value set with the bits *FF_DUR[5:0]* (see **FREE_FALL** (5Dh) register)

#### Configuration registers
**WAKE_UP_DUR** (5Ch) : Free-fall, wakeup and sleep mode functions duration setting register
* FF_DUR5 : last bit of Free Fall duration config, see **FREE_FALL** (5Dh) for complete config.

**FREE_FALL** (5Dh) : Free-fall function duration setting register
* FF_DUR[4:0] : Free-fall duration event. For the complete config bit *FF_DUR5* of **WAKE_UP_DUR** (5Ch) has to be se too
* FF_THS[2:0] : Free fall threshold setting

#### Interrupt registers
**TAP_CFG2** (58h) : Enables interrupt and inactivity functions, and tap recognition functions
* INTERRUPTS_ENABLE : Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity)

**MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)
* INT1_FF : Routing of free-fall event on INT1

**MD2_CFG** (5Eh) : Functions routing on INT2 register (r/w)
* INT2_FF : Routing of free-fall event on INT2


INT1_FF bit of the MD1_CFG register to 1 or the
INT2_FF bit of the MD2_CFG register to 1; it can also be checked by reading the FF_IA bit of the
WAKE_UP_SRC register.
### 3. Orientation

### 4. Tap (single and double)

### 5. Tilt

### 6. Wake up

If the input accelerometer data is bigger than the configured threshold, an interrupt is generated.
Can also be configured as an absolute wake_up with respect to a programmed position

You may choose between two filters on the accelerometer data : slope or high pass

Select minimum duration of a wake up event based on several parameters : 
    - Threshold : limit above which an acceleration is potentially a wake up signal
    - Threshold : weight (To lighten or weighten the duration )
    - Duration : number of periods to 


The wake-up interrupt signal is generated if a certain number of consecutive filtered data exceed the configured
threshold

#### Registers 
**CTRL1_XL**

**TAP_CFG0** (56h) : 
* SLOPE_FDS

**TAP_CFG2** (58h) : 

**WAKE_UP_SRC** (1Bh) : Wake-up interrupt source register (r)
* SLEEP_CHANGE_IA
* FF_IA
* SLEEP_STATE
* WU_IA 
* X_WU
* Y_WU
* Z_WU

**WAKE_UP_THS** (5Bh) : Single/double-tap selection and wake-up configuration (r/w)
* USR_OFF_ON_WU
* WK_THS[5:0]

**WAKE_UP_DUR** (5Ch) : Free-fall, wakeup and sleep mode functions duration setting register (r/w)
* WAKE_DUR[1:0]
* WAKE_THS_W


**X_OFS_USR** (73h) : Accelerometer X-axis user offset correction (r/w). The offset value set in the X_OFS_USR offset register is
internally subtracted from the acceleration value measured on the X-axis

**Y_OFS_USR** (74h) : Accelerometer Y-axis user offset correction (r/w). The offset value set in the Y_OFS_USR offset register is
internally subtracted from the acceleration value measured on the Y-axis.

**Z_OFS_USR** (75h) : Accelerometer Z-axis user offset correction (r/w). The offset value set in the Z_OFS_USR offset register is
internally subtracted from the acceleration value measured on the Z-axis.


**MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)







---
### Some stuff about the MLC we might include

        #### Associated registers

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

        #### Interrupts
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

