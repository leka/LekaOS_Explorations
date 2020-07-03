# LKExp - Sensors - LSM6DSOX - Embedded_Features_Test

This project aims to test and document the embedded features included in the LSM6DSOX sensor from STMicroelectronics.
There are 2 parts to the project, a code example that allows to test most of the features, and a documentation about the embedded features.

---
# Code use 

Change the `#define` macro values in the main.cpp file to select the features you want to test.

The 6D_position and the tap features were not working with precision on our side despite extensive testing of the settings, so they might need further testing.

---
# Documentation

This document tries to give a first approach to the embedded features included on the LSM6DSOX Sensor from STMicroelectronics.
We tried to extract the basic info about each feature and put it in an easy to understand way.

### We documented 6 features :
1. [Sleep State](#1.-Sleep-State-detection)
2. [Free Fall](#2.-Free-fall)
3. [Orientation](#3.-Orientation)
4. [Tap and Double Tap](#4.-Tap-(single-and-double))
5. [Tilt](#5.-Tilt)
6. [Wake up](#6.-Wake-up)


## Information sources

#### LSM6DSOX and its MLC
* **LSM6DSOX Datasheet** : [link](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf) iNEMO inertial module: always-on 3D accelerometer and 3D gyroscope
* **AN5272** : [link](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf) LSM6DSOX: always-on 3D accelerometer and 3D gyroscope


#### Software examples (from ST)
[Most embedded features basic implementations](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsox_STdC/example)

#### Unanswered questions
Some of the features seem to have an erratic behavior, we did not have enough time to really understand what where the consequences of the settings we decided to use.

## General info

## Important info to be able to work
* the INT1 pin on the LSM6DSOX has to be set at 0 at startup for the I2C not to be disabled. If the board hasn't a pull-down on that pin, you have to set it as a DigitalOut LOW (if you want to use INT1, set it as a DigitalInOut, output, low. Then once the sensor has started, disable I3C via its internal functions to force I2C and then set INT1 pin as input)

* After the device is powered up, it performs a 10 ms (maximum) boot procedure. During the boot time the registers are not accessible.

* INT2 signals can be redirected on INT1

* Features trigger interrupts on th sensor, you can check their state via IRQ or simply polling the interrupt status register


---
## Configure Embedded features

### 1. Sleep State detection
Similar to wake up detection 
If the input accelerometer data is lower than the configured threshold for a programmable time, an inactivity interrupt is generated.

When the inactivity interrupt occurs it's possible to set a change for the accelerometer and gyroscope to put them in an energy-saving mode.
The gyroscope can even be switched off since the detection of activity occurs on acelererometer data, the XL ODR can still be set to a minimum (12,5Hz).

You may choose between two filters on the accelerometer data : slope or high pass

For the inactivity signal : 
    - Sleep duration : Time limit after which an inactivity interrupt can be generated if the XL data was under the wake up threshold during that time 
    - Mode set : In which mode to put the accelerometer and the gyroscope in case of inactivity


For the wake up signal :
Select minimum duration of a wake up event based on several parameters : 
    - Threshold : limit above which an acceleration is potentially a wake up signal
    - Threshold : weight (To lighten or weighten the duration )
    - Duration : number of periods to 


The wake-up interrupt signal is generated if a certain number of consecutive filtered data exceed the configured threshold

#### Registers 


**TAP_CFG0** (56h) : Activity/inactivity functions, configuration of filtering, and tap recognition functions (r/w).
* SLOPE_FDS
* LIR
* INT_CLR_ON_READ

**TAP_CFG2** (58h) : Enables interrupt and inactivity functions, and tap recognition functions
* INTERRUPTS_ENABLE : Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity)

**WAKE_UP_SRC** (1Bh) : Wake-up interrupt source register (r)
* SLEEP_CHANGE_IA
* SLEEP_STATE
* WU_IA

**WAKE_UP_THS** (5Bh) : Single/double-tap selection and wake-up configuration (r/w)
* USR_OFF_ON_WU
* WK_THS[5:0]

**WAKE_UP_DUR** (5Ch) : Free-fall, wakeup and sleep mode functions duration setting register (r/w)
* WAKE_DUR[1:0]
* WAKE_THS_W
* SLEEP_DUR[3:0]


**MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)
* INT1_SLEEP_CHANGE
* INT1_WU


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

The orientation embedded feature indicates when the orientation of one of the device's axis is pointing upward.
The interrupt is generated when the device switches from one orientation to another, and not reasserted while the position is maintained.

All 3 axis can be high or low, when the axis is pointing downward it's considerated as low and upward as high. 

A 4-Dimension mode can also be enabled in order to suppress Z-axis orientation changes if needed. 


#### Registers

**TAP_CFG0** (56h) : Activity/inactivity functions, configuration of filtering, and tap recognition functions (r/w).

**TAP_CFG2** (58h) : Enables interrupt and inactivity functions, and tap recognition functions
* INTERRUPTS_ENABLE : Enable basic interrupts (6D/4D, free-fall, wake-up, tap, inactivity)

**TAP_THS_6D** (59h) : Portrait/landscape position and tap function threshold register (r/w).
* D4D_EN
* SIXD_THS[1:0]

**CTRL8_XL** (17h)
* LOW_PASS_ON_6D

**MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)
* INT1_6D



### 4. Tap (single and double)
This embedded feature help to recognize tap and double tap on the sensor, for exemple on a man-machine interface.
An interrupt can be generated in case of a tap in any direction with a parameter :
    - Shock duration : Duration of a shock, which mean the time a tap must last to be detected, if the xl value is not gone below the threshold value during this time, the tap is not detected.
    
It can also be set to a double tap functionnality with some parameters: 
    - Quiet duration : minimum time ocuring before a second tap is detected as a double tap
    - Duration  :  maximum time ocuring after which second tap is not detected as a double tap

It is also possible to prioritize the axis along which taps will be considered first.

#### Registers

**TAP_CFG0** (56h) : Activity/inactivity functions, configuration of filtering, and tap recognition functions (r/w).
* TAP_X_EN
* TAP_Y_EN
* TAP_Z_EN

**TAP_CFG1** (57h) : Tap configuration register (r/w)
* TAP_PRIORITY_[2:0]
* TAP_THS_X_[4:0]

**TAP_CFG2** (58h) : Enables interrupt and inactivity functions, and tap recognition functions
* INTERRUPTS_ENABLE
* TAP_THS_Y_[4:0]

**TAP_THS_6D** (59h) : Portrait/landscape position and tap function threshold register (r/w).
* TAP_THS_Z_[4:0]

**INT_DUR2** (5Ah) : Tap recognition function setting register (r/w).
* DUR[3:0]
* QUIET[1:0]
* SHOCK[1:0]

**WAKE_UP_THS** (5Bh) : Single/double-tap selection and wake-up configuration (r/w)
* SINGLE_DOUBLE_TAP

**MD1_CFG** (5Eh): Functions routing on INT1 register (r/w)
* INT1_SINGLE_TAP
* INT1_DOUBLE_TAP

### 5. Tilt

This embedded feature detect when the device has been tilted over a given degree threshold (35°) from the initial position. 
Once this angular threshold is passed, an interrupt is generated and the actual position is set as initial position for the next possible tilt.


#### Registers

**FUNC_CFG_ACCESS** (01h) : Enable embedded functions register (r/w)
* FUNC_CFG_ACCESS

**EMB_FUNC_EN_A** (04h) : Embedded functions enable register (r/w)
* TILT_EN

**EMB_FUNC_INT1** (0Ah) : INT1 pin control register (r/w). 
Each bit in this register enables a signal to be carried through INT1. 
The pin's output will supply the ORcombination of the selected signals
* INT1_TILT

**PAGE_RW** (17h) : Enable read and write mode of advanced features dedicated page (r/w)
* EMB_FUNC_LIR

**MD1_CFG** (5Eh) : Functions routing on INT1 register (r/w)
* INT1_EMB_FUNC


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

**TAP_CFG0** (56h) : 
* SLOPE_FDS
* LIR
* INT_CLR_ON_READ

**TAP_CFG2** (58h) : 
* INTERRUPTS_ENABLE

**WAKE_UP_SRC** (1Bh) : Wake-up interrupt source register (r)
* SLEEP_CHANGE_IA
* SLEEP_STATE
* WU_IA 

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
