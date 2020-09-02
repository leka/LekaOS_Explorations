cd BOOTLOADER
mbed compile

cd ../APPLICATION
mbed compile 

openocd -f interface/stlink.cfg -c "transport select hla_swd" -f target/stm32f7x.cfg -c "program ./BUILD/DISCO_F769NI/GCC_ARM/APPLICATION.bin 0x08000000" -c "reset" -c "exit"

cd ..