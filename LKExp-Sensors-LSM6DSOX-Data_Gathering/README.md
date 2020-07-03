# LKExp - Sensors - LSM6DSOX - Data_Gathering



This app aims to get data logs from the sensor in order to create new **MLC** models. This is to avoid using Unico since it does not work on other hardware than a few evaluation boards from ST.

Once you have used this app to get a data log from the sensor, you need to follow the configuration steps of the MLC. A lot more information about this can be found in the LKExp-Sensors-LSM6DSOX-MLC_Mbed_Integration project on this same repository.



## Code 
The functionnalities are implemented in a class called **DataGatherer**.
All global methods, implying those having an effect on the whole sensor, have been implemented in this class for ease of use purposes.

The way it works currently is to use a Ticker to get and send the data at the given rate over serial.


## Using the app

### Info about the app
The app runs on the mbed board and allow to configure the data gathering via serial.

There are 5 commands : 

 * start : start sending the data on serial
 * stop : stop sending data
 * rate \<data rate\> : sets the data rate (limited to 104Hz (max of MLC))
 * range \<device\> \<data range\> : sets the range (full scale) of the device
 * help : prints a small help 

The program is preconfigured with these ranges for the sensors:
* 2G for the accelerometer
* 500 DPS for the gyroscope

The data rate is set to 26Hz initially, you can change it via the corresponding command.

### On the computer side
You can use any serial terminal to get the data logs. However, for speed reasons, the app runs serial at 230 400 bauds, you need to configure you serial terminal to this speed.

On linux you can use the command `mbed sterm -b 230400` to call the mbed serial terminal at the right baud rate

To save the data from the serial terminal, you can pipe the console to a file :
`$ mbed sterm -b 230400 | tee <filename>`
⚠️ If you do not deactivate the local echo on your terminal, when typing "stop" command, the letters will be scattered in the binary data
To avoid that hit `CTRL` + `E` once mbed sterm is running


⚠️ Note that right now the code is not very well optimized . It is set to send the data using a printf() functionnality, but with the higher data rates, this leads to application crash.

There is a workaround that consists on sending data in a **binary format** which is way quicker.
However we did not have the time to write an app to convert from binary to CVS.

To send data in binary modify the `onTick()` method of the DataGatherer class. The option is already there, uncomment it and comment the call to `printData()`. 


## Ressources
* [LSM6DSOX datasheet](https://www.st.com/resource/en/datasheet/lsm6dsox.pdf)
* [LSM6DSOX Sensor Application Note AN5272](https://www.st.com/resource/en/application_note/dm00571818-lsm6dsox-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf)
* [LSM6DSOX MLC Application Note AN5259](https://www.st.com/content/ccc/resource/technical/document/application_note/group1/5f/d8/0a/fe/04/f0/4c/b8/DM00563460/files/DM00563460.pdf/jcr:content/translations/en.DM00563460.pdf)
* [Machine Learning Core info](MLC_info.md)

