# LKExp-mbed-USB_and_QSPI_test

## Goal

From **LKExp-Bootloader-DISCO_F769NI-USB_receive_file** as a base, receive a file via USB serial and store it into the external flash

## Resources

[Mbed OS QSPI tutorial](https://os.mbed.com/docs/mbed-os/v6.2/apis/spi-apis.html)

[Mbed OS QSPI class reference](https://os.mbed.com/docs/mbed-os/v6.2/mbed-os-api-doxy/classmbed_1_1_q_s_p_i.html)

## Result

⚠️ Reading/writing on the external flash not working: reading returns only `0x00` characters
