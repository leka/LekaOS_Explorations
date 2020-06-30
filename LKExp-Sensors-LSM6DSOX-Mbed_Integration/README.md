# LKExp - Sensors - LSM6DSOX - MLC_Mbed_Integration

## Goal
This Mbed project implements a class for the LSM6DSOX **Machine Learning Core**.

There are several goals for this project :
* Compile and understand as much data as possible about how the MLC works and how to interact with it.
    This data can be found in the [Machine Learning Core info (MLC_info.md)](MLC_info.md) file of this same repository

* Find a way be able to configure the MLC with a .h configuration file obtained via STM **Unico** software and joined to the project at compile time.
    As of now the .h file is *lsm6dsox_six_d_position.h* which was provided by ST (see the [Ressources](#Ressources) section)
    The example used to create the MLC configuration method was inspired by the ST example available in the examples Github [repository](https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/master/lsm6dsox_STdC/example/lsm6dsox_mlc.c)

* Start to create an abstraction layer to communicate with the MLC, the different methods of the LSM6DSOX_MachineLearningCore class aim to do that.


## Configuration of the MLC

To configure the MLC, the  `configureMLC()` method from the `MachineLearningCore` class reads the `ucf_line_t` variable from the *.h* configuration file line by line. Each line corresponds to a register address from the MLC and the value to write on it.

The movement recognition currently implemented is a 6D position recognition tree. Everytime the sensor will face a different direction, an interrupt is generated and the current position is printed. If the position is not close enough to one of the 6 main orientations, "None" will be printed.


## Ressources
* [LSM6DSOX datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf)
* [LSM6DSOX Sensor Application Note AN5272](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf)
* [LSM6DSOX MLC Application Note AN5259](https://www.st.com/content/ccc/resource/technical/document/application_note/group1/5f/d8/0a/fe/04/f0/4c/b8/DM00563460/files/DM00563460.pdf/jcr:content/translations/en.DM00563460.pdf)
* [Machine Learning Core info](MLC_info.md)

