## Video player on STM32F769I-discovery

Plays a Mjpeg AVI video.

Contains :
- Source files in Src folder
- Header files in Inc folder
- STM32 Cube IDE project in CubeIDEProject folder

The project uses stm32Cube FW F7 package, you need to set the FW_F7 variable to the location of the package on your computer.

Open project, then go to Project > Properties > Resource > Linked Resources , here set FW_F7 variable.
Then go to Project > Poperties > C/C++ Build > Environment , here set FW_F7 variable again.

Then build and flash.
