# LKExp - Update DISCO_F769NI via Pelion

## How to run

Deploy all necessary libs, then add API key (not given here because it is a public repo).

Compile and flash if it is the first time, compile and update otherwise.

To update, you can apply the command mbed dm update device -D <Device_ID> where the Device ID is given by the flashed board at booting when connected to Internet.

To check if the update actually happend, you can change state of LED2 in main.cpp in checkAlive function.