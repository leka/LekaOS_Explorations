# MLC documentation

This document is used as a "dump" of all the things we are learning about the MLC.
The idea is to keep the info extracted from the Datasheet and the Application note safe. We will be trying to put things in order as we advance in the project

## Information sources

#### LSM6DSOX and its MLC
* **LSM6DSOX Datasheet** : [link](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf) iNEMO inertial module: always-on 3D accelerometer and 3D gyroscope
* **AN5272** : [link](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf) LSM6DSOX: always-on 3D accelerometer and 3D gyroscope
* **AN5259** : [link](https://www.st.com/content/ccc/resource/technical/document/application_note/group1/5f/d8/0a/fe/04/f0/4c/b8/DM00563460/files/DM00563460.pdf/jcr:content/translations/en.DM00563460.pdf) LSM6DSOX: Machine Learning Core

#### Hardware
* **UM2435** [link](https://www.st.com/resource/en/user_manual/dm00517423-bluetooth-low-energy-and-802154-nucleo-pack-based-on-stm32wb-series-microcontrollers-stmicroelectronics.pdf) : P-NUCLEO-WB55 user manual
* **UM2121** [link](https://www.st.com/resource/en/user_manual/dm00333132-getting-started-with-the-xnucleoiks01a2-motion-mems-and-environmental-sensor-expansion-board-for-stm32-nucleo-stmicroelectronics.pdf) : X-NUCLEO-IKS01A2 user manual
* STEVAL-MKI197V1 : Data brief (no reference)

#### Software examples (from ST)
* Configuring the MLC : [link](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/tree/master/lsm6dsox_STdC/example/lsm6dsox_mlc.c)
The _lsm6dsox_mlc.c_ file does a basic config of the MLC starting from a pre-made .h which consists on a _ucf_line_t_ variable extracted from a _.ucf_ file.

* Pre-made .ucf and .h : [link](https://github.com/STMicroelectronics/STMems_Machine_Learning_Core/tree/master/lsm6dsox)
These folders contain 6 pre-made configuration files as if you just got them from unico. .h files that you can directly put in you project are also given.

* Configuration example step by step : [link](https://github.com/STMicroelectronics/STMems_Machine_Learning_Core/tree/master/configuration_example)
Here you can find all the files from the data logs to the final _.ucf_ file, showing what each step of the pre-configuration process will give you 


## General info
* the MLC has 8 trees
* max 256 leaves (each leaf is a detected value)
* the trees do not have a defined size for what we know
* selecting a particular tree to change does not seem possible, all the MLC has to be written again

## Important info to be able to work
(we maybe could find a better place for this)
* the INT1 pin on the LSM6DSOX has to be set at 0 at startup for the I2C not to be disabled. If the board hasn't a pull-down on that pin, you have to set it as a DigitalOut LOW (if you want to use INT1, set it as a DigitalInOut, output, low. Then once the sensor has started, disable I3C via its internal functions to force I2C and then set INT1 pin as input)

* ucf writing apparently activates the interrupts on INT1

* After the device is powered up, it performs a 10 ms (maximum) boot procedure. During the boot time the registers are not accessible.

* INT2 signals can be redirected on INT1

* It seems the configuration of the trees is reset or lost on power loss


## Getting a ucf file from a data set 
1. Collect the data
To get the ucf ( Unico Configuration File), you will need to use the Unico GUI software as well as a machine learning software like Weka that can generate a decision tree (There are others but weka is recommended). You can follow those video guides (5 videos) : [ST guide for LSM6DSOX MLC setup](https://www.youtube.com/watch?v=xn92M_VSv0o)
The first step will be to get your movement recognition data set ready, for this you will need to record a correct amount of data corresponding to one movement.
_At this point it is worth noting that for more complicated movements, you will need to use the Finite State Machine fonction of the LSM6DSOX sensor in addition to the Machine Learning Core to differenciate between complex movements._
An important thing you need to know is if your board compatible directly with Unico, for example an ST test board connected to a compatible Nucleo board. In this case you can register your data set directly from Unico [video](https://www.youtube.com/watch?v=xhcq0MMdGiY&t=1s). However if you're using a board that is not compatible with Unico, you will ahave to use Unico in "offline" mode, not communicating with the card. You will nevertheless be able to generate a ucf from a data set, but for the creation of your dataset, and furthermore a dataset compatible with Unico (CSV for example), you will have to do it yourself. We will however try to provide something able to register a data set if we can.
2. Start the configuration and create the arff file to use in a decision tree generator
When your different datasets each corresponding to a movement are recorded. You will need to go in the MLC tab of he Unico GUI : There you just have to add your differents data set and label them (for example with the name of the movement corresponding), it's quite intuitive. Once this is done, go in the configuration sub-tab and follow the different steps, selecting wich data features to use, if you want to use a filter, etc [video](https://www.youtube.com/watch?v=NRvoH6jmiys). 
3. Create a decision tree from Weka
Once you created a arff file, you will need to switch to weka to generate the decision tree, following this : [video](https://www.youtube.com/watch?v=fZEKBWfCyg4).
4. Finish the configuration with the decision tree
To finish with the ucf file creation, go back to the MLC configuration on Unico GUI and define your decision tree [video](https://youtu.be/yvoBt9zl5Ws).
5. (Mbed target) Change the the ucf file to a header file
The last step to be able to use the ucf file, which define what registers need to be written with which data, in an mbed implemented code, it to change a bit his format and make it into a header file that may be included in you code. This is possible using once again Unico GUI, on the main page in the "options" tab, select the button "C code generation" and select your ucf file.


## working with xl and gy
* xl or gy only needed if the features for the trees use them
* if used, xl or gy need to have a higher or equal ODR than the MLC


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
    * LIR : latch basic interrupts

* **TAP_CFG2** (58h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=83) page 83
    * INTERRUPTS_ENABLE : Enable basic interrupts (on embedded functions)
    
* **MD1_CFG** (5Eh) and **MD2_CFG** (5Fh) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=88) page 88 et 89
    * INT*x*_EMB_FUNC (*x* = 1 or 2) : Embedded functions routing on INT1 and INT2 respectively

* **PAGE_RW** (17h) : [datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf#page=124) page 124
    * EMB_FUNC_LIR : Latched Interrupt mode for Embedded Functions (this includes the MLC)
