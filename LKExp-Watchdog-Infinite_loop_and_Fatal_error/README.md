# LKExp - Watchdog - Infinite loop and Fatal error

Quick try of the watchdog to reset the board when infinite loop and fatal error occur.

Final goal is to prevent freeze of the robot.

⚠️ Last version of mbed-os (commit : ad973f2) remove .hex file generation.
The loss is probabily caused by this commit : https://github.com/ARMmbed/mbed-os/commit/5d245ad5537bc8c54795a87549258a6c22df5087
mbed_app.json was adapted to generate .hex file instead of .bin file.
