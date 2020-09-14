# LKExp - Watchdog - Infinite loop and Fatal error

Quick try of the watchdog to reset the board when infinite loop and fatal error occur.

Final goal is to prevent freeze of the robot.

⚠️ Last version of mbed-os (commit : ad973f2) remove .hex file generation.
The loss is probabily caused by this commit : https://github.com/ARMmbed/mbed-os/commit/5d245ad5537bc8c54795a87549258a6c22df5087
mbed_app.json was adapted to generate .hex file instead of .bin file.

## Current issue

Add the two types of "freeze" : infinite loop and fatal error. They can be selected with the button USER on DiscoF769.
Also, two kinds of tools can be used to refresh the watchdog : EventQueue and Ticker.

## Results

When use Ticker and Infinite loop : watchdog is never triggered.
When use Ticker and Fatal error : watchdog reboot the board.
When use EventQueue and Infinite loop : Program seems to be stucked in `dispatch()` function.
When use EventQueue and Fatal error : Program seems to be stucked in `dispatch()` function.
